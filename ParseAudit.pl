#!/usr/bin/perl

use strict;
use warnings;
use DBI;
use POSIX qw(strftime);
#use Proc::Daemon;
use Config::IniFiles;

my $dbh=undef;
my $command=undef;
my $sql=undef;
my $fh=undef;
my $host=undef;
my $waitSeconds=10;
my $quit = 0;




sub Wait 
{
	my $i = 0;
	print "Waiting for $waitSeconds seconds\n";
	while(($i<$waitSeconds) and (!$quit)) 
	{
		sleep 1;
		$i++;
	};
}

sub SendAuditSignal()
{
	my $error=1;

	while(($error) and (!$quit))
	{
		print "Send USR1 signal to WORM process\n";
		$error=`pkill --signal USR1 WORM 2>&1`;
		if ($error)
		{
			print "Cannot send USR1 signal: $error";
			&Wait;	
		}
	};
}

sub ConnectDatabase()
{
	$dbh=0;

	while((!$dbh) and (!$quit))
	{
		print "Trying to connect database ($host)\n";
		$dbh = DBI->connect("dbi:mysql:WORM;host=$host",'audit','audit1708$');
		if (!$dbh)
		{
			warn "Failed to connect database: $DBI::errstr\n";
			&Wait;	
		}
	};

}

sub CloseDatabase()
{
	if ($dbh)
	{
		print "Close database\n";
		$dbh->disconnect;
		&Wait;
	}
}

# 0=OK, 1=File Error, 2=SQL Error
sub ProcessFile()
{
	my $file=$_[0];
	my $fullFileName=undef;
	my @parts=undef;
	my $error=0;


	print "Trying to open file $file\n";
	$fullFileName = "/var/log/$file";

	if (!(open($fh, '<:encoding(UTF-8)', $fullFileName)))
	{
		warn "Could not open file: $!";
		$error= 1;
	}

	while ((my $row = <$fh>) and ($error==0) and (!$quit)) 
	{
		chomp $row;
		my @parts=split('\|',$row);
		my $datestring = strftime "%Y-%m-%d %H:%M:%S", localtime($parts[0]);

		my $id=$parts[1];
		my $operation=$parts[2];
		my $entity=$parts[3];
		my $result=$parts[4];
		my $target=$parts[5];
		my $value=$parts[6];
		my $uid=$parts[7];
		my $gid=$parts[8];
	
		#print "$row\n";

		$sql="CALL WriteAudit( '$datestring',$id,'$operation', '$entity','$result', '$target','$value',$uid,$gid);";
		print "Insert audit in database: $sql\n";
		
		if (!($command=$dbh->prepare($sql)))
		{ 
			$error=2;
			warn "Failed to prepare stored procedure: $!";
		} 
		elsif (!($command->execute()))
		{
			$error=2;
			warn "Failed to execute stored procedure: $!";
		} 
		else 
		{
			$command->finish();
		}
		
	}

	#print "Close file\n";
	if ($error!=1) 
	{
		close($fh);
	}

	#print "Trying to Remove audit file\n"
	if ($error==0)
	{
		unlink $fullFileName or warn "Failed to remove file: $!";
	}	

	return $error;
}
sub ParseFiles()
{
	my @files=undef;
	my $file=undef;
	my $error=undef;

	print "Trying to parse all audit files\n";
	opendir(DIR, "/var/log/") or die "Cannot open directory /var/log/\n"; 
	@files = grep(/WORM_Audit-[0-9]+\.log$/,readdir(DIR)) or warn "Cannot enumerate files\n";
	closedir(DIR);

	foreach $file (@files) 
	{
		$error=&ProcessFile($file);
		if (($error==2) or ($quit==1)) 
		{
			last;
		}
	}


}


#Proc::Daemon::Init;


$SIG{TERM} = sub { print "Catch TERM signal\n"; $quit = 1; };

my $cfg = Config::IniFiles->new( -file => "/etc/WORM.conf" ,-handle_trailing_comment =>1);


$host=$cfg->val( 'General', 'Database' );

while(!$quit)
{
	&SendAuditSignal;	
	&ConnectDatabase;	
	&ParseFiles;
	&CloseDatabase;	
}

