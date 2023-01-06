Name: memwatch
Version: 1.0
Release: 2%{?dist}
Summary: Dynamic user memory watchdog

License: MIT
Source0: %{name}-%{version}.tar.gz
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-XXXXXX)

BuildRequires: make

%description
memwatch allows a multi-user system to avoid thrashing by preventing user processes from consuming more than a set percentage of the system memory. 

%global debug_package %{nil}

%prep
%setup -q

%build
make all

%install
install --directory $RPM_BUILD_ROOT/usr/share/doc/memwatch
install --directory $RPM_BUILD_ROOT/usr/sbin
install --directory $RPM_BUILD_ROOT/usr/lib/systemd/system
install --directory $RPM_BUILD_ROOT/usr/libexec/memwatch

install -m 644 LICENSE $RPM_BUILD_ROOT/usr/share/doc/memwatch
install -m 644 README.md $RPM_BUILD_ROOT/usr/share/doc/memwatch
install -m 755 memwatch $RPM_BUILD_ROOT/usr/sbin
install -m 755 hook.example $RPM_BUILD_ROOT/usr/libexec/memwatch
install -m 644 memwatch.service $RPM_BUILD_ROOT/usr/lib/systemd/system

%clean
rm -rf $RPM_BUILD_ROOT

%files
/usr/share/doc/memwatch/LICENSE
/usr/share/doc/memwatch/README.md
/usr/sbin/memwatch
/usr/libexec/memwatch/hook.example
/usr/lib/systemd/system/memwatch.service

%changelog
