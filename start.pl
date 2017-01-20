#!/usr/bin/perl
use strict;
use warnings;
use Getopt::Long;

my @users;
my @shares;
my $file;
my $folder;

$SIG{TERM} = sub { die "Received TERM signal, exiting..." };

GetOptions ("user=s"   => \@users,      # string
            "share=s"  => \@shares)   # flag
or die("Error in command line arguments\n");

print("Creating users...\n");
foreach my $user (@users) {
	if ( $user =~ /^([a-z_][a-z0-9_-]*[\$]?):([^:]+):(\d+)/)
	{
		print "Login:$1 Password:$2 ID:$3\n"; 
		system("groupadd -g $3 $1");
		system("useradd -s /sbin/nologin $1 -M -u $3 -g $3");
		system("echo $2 | passwd $1 --stdin");
		system("printf \"$2\n$2\" |  smbpasswd -a -s $1");
	}
	else
	{
		warn "Invalid user definition: $user"; 
	}
}

print("Creating shares...\n");

rename("/etc/samba/smb.conf","/etc/samba/smb.old") or die "Could not backup samba configuration file !";
open($file, '>', "/etc/samba/smb.conf") or die "Could not open samba configuration file !";

print $file "[global]\n";
print $file "workgroup = WORKGROUP\n";
print $file "server string = WORM Server\n";
print $file "\n";
print $file "# log files split per-machine:\n";
print $file "log file = /var/log/samba/log.%m\n";
print $file "# maximum size of 50KB per log file, then rotate:\n";
print $file "max log size = 50\n";
print $file "\n";
print $file "#security\n";
print $file "security = user\n";
print $file "passdb backend = tdbsam\n";
print $file "\n";
print $file "#printers\n";
print $file "load printers = no\n";
print $file "passdb backend = tdbsam\n";
print $file "map to guest = Bad User\n";
print $file "unix password sync = yes\n";
print $file "\n";
print $file "#shares\n";

foreach my $share (@shares) {
	if ( $share =~ /^([A-Za-z0-9_-]+):((?:\/[^\/]+)*\/):([a-z_][a-z0-9_-]*[\$]?)/)
	{
		$folder="/mnt/WORM".$2;
		
		print "Share:$1 Path:$2 User:$3\n"; 
		print $file "[$1]\n";
		print $file "path = $folder\n";
		print $file "comment = WORM share\n";
		print $file "read only = no\n";
		print $file "browseable = yes\n";
		print $file "guest ok = no\n";
		print $file "valid users = $3\n";
		print $file "admin users = Bad User\n";
		print $file "\n";
		
		#if (!-d $folder) {
		#	print("Shared folder doesn't exists creating...\n");
		#	mkdir($folder) or warn "Failed to create folder $folder";
		#	system("chown $3:$3 $folder");
		#}
    
	}
	else
	{
		warn "Invalid share definition: $share"; 
	}


}

close $file;

print("Launching WORM daemon...\n");
system("/usr/local/bin/WORM -o allow_other,default_permissions");

print("Launching SAMBA daemon...\n");
system("smbd");

print("Waiting for TERM signal...");
sleep;
