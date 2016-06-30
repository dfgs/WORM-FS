#!/usr/bin/env bats

setup() {
	echo "setup"
}

teardown() {
	echo "teardown"
}

failing_function() {
  echo "not good"
  return 1
}


@test "Config backup" {
	mv /etc/WORM.conf /etc/WORM.old && cp TestWithAutoCommit.conf /etc/WORM.conf	
}
@test "Restart service" {
	systemctl restart WORM
}
@test "Create directory" {
	mkdir /mnt/WORM/Test
}
@test "Overwrite directory" {
	run mkdir /mnt/WORM/Test
	[ $status -ne 0 ]
}
@test "Open/release directory" {
	ls /mnt/WORM/Test
}
@test "Remove directory" {
	rmdir /mnt/WORM/Test
}

@test "Create directory and delay" {
	mkdir /mnt/WORM/Test
	sleep 2
}
@test "Rename directory with delay" {
	run mv /mnt/WORM/Test /mnt/WORM/Test2
	[ $status -ne 0 ]
}

@test "Remove directory with delay" {
	run rmdir /mnt/WORM/Test
	[ $status -ne 0 ]
}
@test "Overwrite directory with delay" {
	run mkdir /mnt/WORM/Test
	[ $status -ne 0 ]
}


@test "Create file (ret 0)" {
	ls >/mnt/WORM/Test/test.rt0
}

@test "Overwrite file (ret 0)" {
	ls >/mnt/WORM/Test/test.rt0
}

@test "Append file (ret 0)" {
	ls >>/mnt/WORM/Test/test.rt0
}
@test "stat file (ret 0)" {
	stat /mnt/WORM/Test/test.rt0
}
@test "list extended attributes (ret 0)" {
	getfattr /mnt/WORM/Test/test.rt0
}
@test "get extended attribute (ret 0)" {
	getfattr -n user.Retention /mnt/WORM/Test/test.rt0
}
@test "set extended attribute (ret 0)" {
	setfattr -n user.Test /mnt/WORM/Test/test.rt0
}
@test "remove extended attribute (ret 0)" {
	setfattr -x user.Test /mnt/WORM/Test/test.rt0
}

@test "Update access time (ret 0)" {
	touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt0
}
@test "Update modification time (ret 0)" {
	touch -m -t 299901010000.00 /mnt/WORM/Test/test.rt0
}

@test "Change file owner (ret 0)" {
	chown worm:worm /mnt/WORM/Test/test.rt0
}
@test "Change file permissions (ret 0)" {
	chmod o+w /mnt/WORM/Test/test.rt0
}

@test "Rename file (ret 0)" {
	mv /mnt/WORM/Test/test.rt0 /mnt/WORM/Test/test1.rt0
}

@test "Remove file (ret 0)" {
	rm /mnt/WORM/Test/test1.rt0
}








@test "Create file and delay (ret 0)" {
	ls >/mnt/WORM/Test/test.rt0
	sleep 2
}
@test "Overwrite file with delay (ret 0)" {
	ls >/mnt/WORM/Test/test.rt0
}
@test "Append file with delay (ret 0)" {
	ls >>/mnt/WORM/Test/test.rt0
}

@test "stat file with delay (ret 0)" {
	stat /mnt/WORM/Test/test.rt0
}
@test "list extended attributes  with delay (ret 0)" {
	getfattr /mnt/WORM/Test/test.rt0
}
@test "get extended attribute  with delay (ret 0)" {
	getfattr -n user.Retention /mnt/WORM/Test/test.rt0
}
@test "set extended attribute  with delay (ret 0)" {
	setfattr -n user.Test /mnt/WORM/Test/test.rt0
}
@test "remove extended attribute  with delay (ret 0)" {
	setfattr -x user.Test /mnt/WORM/Test/test.rt0
}

@test "Update access time with delay (ret 0)" {
	touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt0
}
@test "Update modification time with delay (ret 0)" {
	touch -m -t 299901010000.00 /mnt/WORM/Test/test.rt0
}

@test "Change file owner with delay (ret 0)" {
	chown worm:worm /mnt/WORM/Test/test.rt0
}
@test "Change file permissions with delay (ret 0)" {
	chmod o+w /mnt/WORM/Test/test.rt0
}

@test "Rename file with delay (ret 0)" {
	mv /mnt/WORM/Test/test.rt0 /mnt/WORM/Test/test1.rt0
}

@test "Remove file  with delay (ret 0)" {
	rm /mnt/WORM/Test/test1.rt0
}

@test "Create file (ret 1)" {
	ls >/mnt/WORM/Test/test.rt1
}
@test "Overwrite file (ret 1)" {
	ls >/mnt/WORM/Test/test.rt1
}
@test "Append file (ret 1)" {
	ls >>/mnt/WORM/Test/test.rt1
}
@test "Remove file (ret 1)" {
	rm /mnt/WORM/Test/test.rt1
}
@test "Create file and delay (ret 1)" {
	ls >/mnt/WORM/Test/test.rt1
	sleep 2
}
@test "Overwrite file with delay (ret 1)" {
	run bash -c "ls >/mnt/WORM/Test/test.rt1"
	[ $status -ne 0 ]
}
@test "Append file with delay (ret 1)" {
	run bash -c 'ls >>/mnt/WORM/Test/test.rt1'
	[ $status -ne 0 ]
}
@test "stat file with delay (ret 1)" {
	stat /mnt/WORM/Test/test.rt1
}
@test "list extended attributes  with delay (ret 1)" {
	getfattr /mnt/WORM/Test/test.rt1
}
@test "get extended attribute  with delay (ret 1)" {
	getfattr -n user.Retention /mnt/WORM/Test/test.rt1
}
@test "set extended attribute  with delay (ret 1)" {
	run setfattr -n user.Test /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "remove extended attribute  with delay (ret 1)" {
	run setfattr -x user.Retention /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Update access time with delay (ret 1)" {
	run touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Update modification time with delay (ret 1)" {
	run touch -m -t 299901010000.00 /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Change file owner (ret 1)" {
	run chown worm:worm /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Change file permissions (ret 1)" {
	run chmod o+w /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}

@test "Rename file with delay (ret 1)" {
	run mv /mnt/WORM/Test/test.rt1 /mnt/WORM/Test/test2.rt1
	[ $status -ne 0 ]
}

@test "Remove file with delay (ret 1)" {
	run rm /mnt/WORM/Test/test.rt1 
	[ $status -ne 0 ]
}
@test "Create file for link tests (ret 0)" {
	ls >/mnt/WORM/Test/test.rt0
}
@test "Create link (ret 0)" {
	ln /mnt/WORM/Test/test.rt0 /mnt/WORM/Test/link.rt0
}
@test "read link (ret 0)" {
	run readlink /mnt/WORM/Test/link.rt0
	[ $status -ne 0 ]
}
@test "ulink (ret 0)" {
	unlink /mnt/WORM/Test/link.rt0
}

@test "Create sym link (ret 0)" {
	ln -s /mnt/WORMTest/test.rt0 /mnt/WORM/Test/link.rt0
}
@test "read sym link (ret 0)" {
	readlink /mnt/WORM/Test/link.rt0
}
@test "ulink sym (ret 0)" {
	unlink /mnt/WORM/Test/link.rt0
}

@test "Create link and delay ret 1)" {
	ln /mnt/WORM/Test/test.rt1 /mnt/WORM/Test/link.rt1
	sleep 2
}
@test "read link with delay (ret 1)" {
	run readlink /mnt/WORM/Test/link.rt1
	[ $status -ne 0 ]
}
@test "ulink with delay (ret 1)" {
	run unlink /mnt/WORM/Test/link.rt1
	[ $status -ne 0 ]
}

@test "Create sym link and delay (ret 1)" {
	ln -s /mnt/WORMTest/test.rt1 /mnt/WORM/Test/link2.rt1
	sleep 2
}
@test "read sym link with delay (ret 1)" {
	readlink /mnt/WORM/Test/link2.rt1
}

@test "ulink sym with delay (ret 1)" {
	unlink /mnt/WORM/Test/link2.rt1
}

@test "Retrieve file state" {
	df /mnt/WORM
}






@test "Clean files for lock test" {
	rm -rf /tmp/Test & rm -rf /tmp/Test2
}

@test "Config change for lock test" {
	rm /etc/WORM.conf && cp TestWithoutAutoCommit.conf /etc/WORM.conf	
}
@test "Restart service for lock test" {
	systemctl restart WORM
}
@test "Create directory without lock" {
	mkdir /mnt/WORM/Test
}
@test "Overwrite directory without lock" {
	run mkdir /mnt/WORM/Test
	[ $status -ne 0 ]
}
@test "Open/release directory without lock" {
	ls /mnt/WORM/Test
}
@test "Remove directory without lock" {
	rmdir /mnt/WORM/Test
}

@test "Create directory and lock" {
	mkdir /mnt/WORM/Test & chmod a-w /mnt/WORM/Test & touch -a -t 299901010000.00 /mnt/WORM/Test
}
@test "Rename directory with lock" {
	run mv /mnt/WORM/Test /mnt/WORM/Test2
	[ $status -ne 0 ]
}

@test "Remove directory with lock" {
	run rmdir /mnt/WORM/Test
	[ $status -ne 0 ]
}
@test "Overwrite directory with lock" {
	run mkdir /mnt/WORM/Test
	[ $status -ne 0 ]
}


@test "Create file without lock (ret 1)" {
	ls >/mnt/WORM/Test/test.rt1
}

@test "Overwrite file without lock (ret 1)" {
	ls >/mnt/WORM/Test/test.rt
}

@test "Append file without lock (ret 1)" {
	ls >>/mnt/WORM/Test/test.rt1
}
@test "stat file without lock (ret 1)" {
	stat /mnt/WORM/Test/test.rt1
}
@test "list extended attributes without lock (ret 1)" {
	getfattr /mnt/WORM/Test/test.rt1
}
@test "get extended attribute without lock (ret 1)" {
	getfattr -n user.Retention /mnt/WORM/Test/test.rt1
}
@test "set extended attribute without lock (ret 1)" {
	setfattr -n user.Test /mnt/WORM/Test/test.rt1
}
@test "remove extended attribute without lock (ret 1)" {
	setfattr -x user.Test /mnt/WORM/Test/test.rt1
}

@test "Update access time without lock (ret 1)" {
	touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt1
}
@test "Update modification time without lock (ret 1)" {
	touch -m -t 299901010000.00 /mnt/WORM/Test/test.rt1
}

@test "Change file owner without lock (ret 1)" {
	chown worm:worm /mnt/WORM/Test/test.rt1
}
@test "Change file permissions without lock (ret 1)" {
	chmod o+w /mnt/WORM/Test/test.rt1
}

@test "Rename file without lock (ret 1)" {
	mv /mnt/WORM/Test/test.rt1 /mnt/WORM/Test/test1.rt1
}

@test "Remove file without lock (ret 1)" {
	rm /mnt/WORM/Test/test1.rt1
}


@test "Create file and lock (ret 1)" {
	ls >/mnt/WORM/Test/test.rt1 & chmod a-w /mnt/WORM/Test/test.rt1 & touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt1
}
@test "Overwrite file with lock (ret 1)" {
	run bash -c "ls >/mnt/WORM/Test/test.rt1"
	[ $status -ne 0 ]
}
@test "Append file with lock (ret 1)" {
	run bash -c 'ls >>/mnt/WORM/Test/test.rt1'
	[ $status -ne 0 ]
}
@test "stat file with lock (ret 1)" {
	stat /mnt/WORM/Test/test.rt1
}
@test "list extended attributes  with lock (ret 1)" {
	getfattr /mnt/WORM/Test/test.rt1
}
@test "get extended attribute  with lock (ret 1)" {
	getfattr -n user.Retention /mnt/WORM/Test/test.rt1
}
@test "set extended attribute  with lock (ret 1)" {
	run setfattr -n user.Test /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "remove extended attribute  with lock (ret 1)" {
	run setfattr -x user.Retention /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Update access time with lock (ret 1)" {
	run touch -a -t 299901010000.00 /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Update modification time with lock (ret 1)" {
	run touch -m -t 299901010000.00 /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Change file owner with lock (ret 1)" {
	run chown worm:worm /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}
@test "Change file permissions with lock (ret 1)" {
	run chmod o+w /mnt/WORM/Test/test.rt1
	[ $status -ne 0 ]
}

@test "Rename file with lock (ret 1)" {
	run mv /mnt/WORM/Test/test.rt1 /mnt/WORM/Test/test2.rt1
	[ $status -ne 0 ]
}

@test "Remove file with lock (ret 1)" {
	run rm /mnt/WORM/Test/test.rt1 
	[ $status -ne 0 ]
}



@test "Clean files" {
	rm -rf /tmp/Test & rm -rf /tmp/Test2
}

@test "Config restore" {
	#rm /etc/WORM.conf && mv /etc/WORM.old /etc/WORM.conf
}



#@test "test for failure" {
#  run failing_function
#
#  [ $status -ne 0 ]
#}