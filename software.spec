Name: worm
Version: 1.0
Release: 1
Summary: WORM compliant file system
PreReq: fuse-libs


Group: Applications/Internet
License: MIT
URL: https://github.com/dfgs/WORM-FS


%define	WORM_SOURCE_FILE	worm
%define	WORM_SOURCE_DIR		%{WORM_SOURCE_FILE}
%define	WORM_VERSION		1.0
%define	WORM_FULLPATH		%{name}-%{version}/%{WORM_SOURCE_DIR}-%{WORM_VERSION}

%define	GETRETENTION_SOURCE_FILE	getretention
%define	GETRETENTION_SOURCE_DIR		%{GETRETENTION_SOURCE_FILE}
%define	GETRETENTION_VERSION		1.0
%define	GETRETENTION_FULLPATH		 %{name}-%{version}/%{GETRETENTION_SOURCE_DIR}-%{GETRETENTION_VERSION}


Source0: %{WORM_SOURCE_FILE}-%{WORM_VERSION}.tar.gz
Source1: %{GETRETENTION_SOURCE_FILE}-%{GETRETENTION_VERSION}.tar.gz


%description
WORM-FS is WORM (Write Once Read Many) compliant file system. Once a file is created, you cannot delete or update it, until the retention period has expired.


%prep 
%setup -c -a 0 -a 1

%build
cd %{_builddir}/%{WORM_FULLPATH}
./configure
make

cd %{_builddir}/%{GETRETENTION_FULLPATH}
./configure
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system
mkdir -p $RPM_BUILD_ROOT/etc
mkdir -p $RPM_BUILD_ROOT/etc/logrotate.d

install %{WORM_SOURCE_DIR}-%{WORM_VERSION}/%{WORM_SOURCE_FILE} $RPM_BUILD_ROOT/usr/local/bin
install %{GETRETENTION_SOURCE_DIR}-%{GETRETENTION_VERSION}/%{GETRETENTION_SOURCE_FILE} $RPM_BUILD_ROOT/usr/local/bin
install %{WORM_SOURCE_DIR}-%{WORM_VERSION}/worm.service $RPM_BUILD_ROOT/usr/lib/systemd/system
install %{WORM_SOURCE_DIR}-%{WORM_VERSION}/worm.conf $RPM_BUILD_ROOT/etc
install %{WORM_SOURCE_DIR}-%{WORM_VERSION}/wormrotate $RPM_BUILD_ROOT/etc/logrotate.d/worm


%files
%defattr(-,root,root)
#%doc README
%attr(644, root, root) %config(noreplace) /etc/worm.conf
%attr(644, root, root) %config(noreplace) /etc/logrotate.d/worm
%attr(644, root, root) %config(noreplace) /usr/lib/systemd/system/worm.service
/usr/local/bin

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Fri Jan 20 2017 Marc GUICHARD <dfgs83@gmail.com> - 1.0-1
- Initial build
