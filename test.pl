#!/bin/perl

use strict;
use warnings;
use Linux::UserXAttr qw/:all/;
use Filesys::Statvfs;
use Term::ANSIColor;

my $passed;
my $failed;
my $autoLock;
my $verbose;

sub assert {
	my ($test,$received,$shouldFail) = @_;

	
	if ($received xor $shouldFail)
	{
		if ($verbose) {
			print "$test: ";
			print color("green"), "PASSED\n", color("reset");
		}
			
		$passed++;
	}
	else
	{
			print "$test: ";
			print color("red"), "FAILED\n", color("reset");
		$failed++;
	}
}

sub lock {
	my ($path,$ret) = @_;

	if ($ret)
	{
		chmod(0444,$path);
		utime(time +$ret * 24 * 60 * 60 ,time+ $ret * 24 * 60 * 60,$path);
	}
}

sub testReadOnly {
	my ($rootDir,$name) = @_;
	my $file;
	my $path;
	
	$path="$rootDir/$name";
	
	if ( -d $path)
	{
		assert( "Open directory" ,opendir($file, $path),0);
		assert( "Read directory" ,defined(readdir($file)),0);
		assert( "Close directory" ,closedir($file),0);
	}
	else
	{
		assert( "Open file" ,open($file, $path),0);
		assert( "Read file" ,defined(<$file>) ,0);
		assert( "Close file" ,close($file),0);
	}
	
	assert("Get file attributes", defined(stat($path)),0);
	#assert("Check file access", defined($result= -W $path),0); does not call access :(
	assert("Get extended attribute",defined(getxattr($path, 'user.Retention')) ,0);
	assert("List extended attributes",defined(listxattr($path)) ,0);
	assert("Get file status info",defined(stat($path)) ,0);
	assert("Get file system info",defined(statvfs($path)) ,0);
	
	
}

sub testWriteOnly {
	my ($rootDir,$name,$shouldFail) = @_;
	my $path;
	my $file;
	
	$path="$rootDir/$name";
	
	if ( -d $path)
	{
		assert( "Overwrite directory" ,mkdir($path),1);
	}
	else
	{
		assert( "Overwrite file", open($file,'>',$path) && close($file),$shouldFail );
		assert( "Append file", open($file,'>>',$path) && (print $file 'test') &&  close($file),$shouldFail );
		assert( "Truncate file", truncate($path,0),$shouldFail );
	}
	
	assert( "Rename file" ,rename($path,"$rootDir/newName") && rename("$rootDir/newName",$path),$shouldFail);

	assert("Set extended attribute", setxattr($path,"user.Test",1234),$shouldFail);
	assert("Remove extended attribute", removexattr($path,"user.Retention"),$shouldFail);
	assert("Change access/modification time (decrease)", utime(0,0,$path),$shouldFail);
	assert("Change access/modification time (increase)", utime(time + 10 * 24 * 60 * 60 ,0,$path),$shouldFail & $autoLock);
		

	
	assert ("Change mod" , chmod("777",$path),$shouldFail); 
	assert ("Change own" , chown($<,$(,$path),$shouldFail); 
	
	if ( -d $path)
	{
		assert( "Remove directory" ,rmdir($path),$shouldFail);
	}
	else
	{
		assert( "Remove file" ,unlink($path),$shouldFail);
	}
	

}


sub testDirectoryWithoutLock {
	my $dir;
	my ($rootDir) = @_;
	
	assert( "Create directory" ,mkdir("$rootDir/Directory"),0);
	testReadOnly($rootDir,'Directory');
	testWriteOnly($rootDir,'Directory',0);
}

sub testDirectoryWithLock {
	my $dir;
	my ($rootDir) = @_;
	
	assert( "Create directory" ,mkdir("$rootDir/Directory"),0);
	if ($autoLock)
	{
		sleep(2);
	}
	else
	{
		lock("$rootDir/Directory",1);
	}
	
	testReadOnly($rootDir,'Directory');
	testWriteOnly($rootDir,'Directory',1);
}

sub testFileWithoutLock {
	my $file;
	my ($rootDir,$ret) = @_;

	assert( "Create file", open($file,'>',"$rootDir/File.rt$ret") && (print $file 'test') && close($file),0 );
	testReadOnly($rootDir,"File.rt$ret");
	testWriteOnly($rootDir,"File.rt$ret",0);
}

sub testFileWithLock {
	my $file;
	my ($rootDir,$ret) = @_;

	assert( "Create file", open($file,'>',"$rootDir/File.rt$ret") && (print $file 'test') && close($file),0 );
	if ($autoLock)
	{
		sleep(2);
	}
	else
	{
		lock("$rootDir/File.rt$ret",$ret);
	}

	testReadOnly($rootDir,"File.rt$ret");
	testWriteOnly($rootDir,"File.rt$ret",$ret);
}

sub testLinkWithoutLock {
	my $file;
	my ($rootDir,$ret) = @_;

	assert( "Create file", open($file,'>',"$rootDir/LinkedFile.rt$ret") && (print $file 'test') && close($file),0 );
	assert( "Create link to file",link("$rootDir/LinkedFile.rt$ret","$rootDir/Link$ret.rt$ret") ,0);
	
	testReadOnly($rootDir,"Link$ret.rt$ret");
	testWriteOnly($rootDir,"Link$ret.rt$ret",0);

	assert( "Clean file", unlink("$rootDir/LinkedFile.rt$ret"),0);
	
}
sub testLinkWithLock {
	my $file;
	my ($rootDir,$ret) = @_;

	assert( "Create file", open($file,'>',"$rootDir/LinkedFile.rt$ret") && (print $file 'test') && close($file),0 );
	assert( "Create link to file",link("$rootDir/LinkedFile.rt$ret","$rootDir/Link$ret.rt$ret"),0 );
	if ($autoLock)
	{
		sleep(2);
	}
	else
	{
		lock("$rootDir/Link$ret.rt$ret",$ret);
	}
	testReadOnly($rootDir,"Link$ret.rt$ret");
	testWriteOnly($rootDir,"Link$ret.rt$ret",$ret);

	assert( "Clean file", unlink("$rootDir/LinkedFile.rt$ret"),$ret);
}

sub testSymLink {
	my $file;
	my ($rootDir,$ret) = @_;

	assert( "Create file", open($file,'>',"$rootDir/LinkedFile.rt$ret") && (print $file 'test') && close($file),0 );
	assert( "Create sym link to file",symlink("$rootDir/LinkedFile.rt$ret","$rootDir/Link$ret.rt$ret"),0 );
	assert( "Read sym link",readlink("$rootDir/Link$ret.rt$ret") ,0);
	assert( "Remove sym link",unlink("$rootDir/Link$ret.rt$ret") ,0);
	

	assert( "Clean file", unlink("$rootDir/LinkedFile.rt$ret"),0);
	
}

sub testAll {
	my ($rootDir)= @_;
	print("\n");
	print("Test directory without lock\n");
	print("---------------------------\n");
	testDirectoryWithoutLock($rootDir);

	print("\n");
	print("Test directory with lock\n");
	print("------------------------\n");
	testDirectoryWithLock($rootDir);

	print("\n");
	print("Test file without lock (rt0)\n");
	print("----------------------------\n");
	testFileWithoutLock($rootDir,0);

	print("\n");
	print("Test file without lock (rt1)\n");
	print("----------------------------\n");
	testFileWithoutLock($rootDir,1);

	print("\n");
	print("Test file with lock (rt0)\n");
	print("-------------------------\n");
	testFileWithLock($rootDir,0);

	print("\n");
	print("Test file with lock (rt1)\n");
	print("-------------------------\n");
	testFileWithLock($rootDir,1);

	print("\n");
	print("Test link without lock (rt0)\n");
	print("----------------------------\n");
	testLinkWithoutLock($rootDir,0);

	print("\n");
	print("Test link without lock (rt1)\n");
	print("----------------------------\n");
	testLinkWithoutLock($rootDir,1);

	print("\n");
	print("Test link with lock (rt0)\n");
	print("----------------------------\n");
	testLinkWithLock($rootDir,0);

	print("\n");
	print("Test link with lock (rt1)\n");
	print("----------------------------\n");
	testLinkWithLock($rootDir,1);

	print("\n");
	print("Test symlink (rt0)\n");
	print("----------------------------\n");
	testSymLink($rootDir,0);
}


$passed=0;$failed=0;$verbose = 0;

if (($#ARGV + 1 > 0) && ($ARGV[0]=="-v")) 
{
	$verbose=1;
}





print("Config backup\n");
`sudo mv /etc/worm.conf /etc/worm.old`;


print("Copy config with autolock\n");
`sudo cp TestWithAutoCommit.conf /etc/worm.conf`;

print("Restart service\n");
`sudo systemctl restart worm`;

	
print("Creating test directory\n");
mkdir("/mnt/WORM/Test") or warn "Cannot create Test directory";

$autoLock=1;
testAll("/mnt/WORM/Test");

print("Cleanning directories\n");
`rm -rf /tmp/Test`;


print("Copy config without autolock\n"); 
`sudo cp TestWithoutAutoCommit.conf /etc/worm.conf`;

print("Restart service\n");
`sudo systemctl restart worm`;

	
print("Creating test directory\n");
mkdir("/mnt/WORM/Test") or warn "Cannot create Test directory";

$autoLock=0;
testAll("/mnt/WORM/Test");

print("Cleanning directories\n");
`rm -rf /tmp/Test`;

print("Config restore\n");
`sudo rm /etc/worm.conf`;

`sudo mv /etc/worm.old /etc/worm.conf`;


print "\n";
print "Tests summary:\n";
print "----------------\n";
print "PASSED tests: $passed\n";
print "FAILED tests: $failed\n";
print "\n";
