Name: worm
Version: 1.0
Release: 1
Summary: WORM compliant file system
PreReq: fuse-libs


Group: Applications/Internet
License: MIT
URL: https://github.com/dfgs/WORM-FS
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
WORM-FS is WORM (Write Once Read Many) compliant file system. Once a file is created, you cannot delete or update it, until the retention period has expired.



%prep
%setup -q

%build
./configure
make

%install
mkdir -p $RPM_BUILD_ROOT/usr/local/bin
mkdir -p $RPM_BUILD_ROOT/etc
mkdir -p $RPM_BUILD_ROOT/usr/lib/systemd/system

install %{name} $RPM_BUILD_ROOT/usr/local/bin
install worm.conf $RPM_BUILD_ROOT/etc
install worm.service $RPM_BUILD_ROOT/usr/lib/systemd/system

%files
%defattr(-,root,root)
#%doc README
%config(noreplace) /etc/worm.conf
%config(noreplace) /usr/lib/systemd/system/worm.service
/usr/local/bin

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Fri Jan 20 2017 Marc GUICHARD <dfgs83@gmail.com> - 1.0-1
- Initial build
