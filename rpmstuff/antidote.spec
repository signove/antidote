# >> macros
# << macros

Name:       antidote
Summary:    IEEE 11073 Protocol Implementation
Version:    2.0.0
Release:    3
Group:      System/Daemons
License:    LGPLv2.1 
URL:        http://www.signove.com
Source0:    antidote.tar.gz
BuildRequires:  dbus-devel
BuildRequires:  dbus-glib-devel

%description
IEEE 11073 Protocol Implementation
for embedded devices.


%package devel
Summary: Development headers for IEEE 20601-11073 Health based applications
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Development headers for IEEE 20601-11073 Health based applications.

%package devel-doc
Summary: Development documentatio for IEEE 20601-11073 Health based applications
Group: Development/Libraries

%description devel-doc
Development documentation for IEEE 20601-11073 Health based applications.


%prep
%setup -q -n %{name}

# >> setup
# << setup

%build
# >> build pre
./autogen.sh
# << build pre

%configure --enable-tests=no

make %{?jobs:-j%jobs}
./create_doc.sh

# >> build post
# << build post
%install
rm -rf %{buildroot}
# >> install pre
# << install pre

mkdir -p %{buildroot}/etc/dbus-1/system.d/
mkdir -p %{buildroot}/usr/share/dbus-1/system-services/
mkdir -p %{buildroot}%{_docdir}/antidote/

cp src/healthd.conf %{buildroot}/etc/dbus-1/system.d/
cp rpmstuff/com.signove.health.service  %{buildroot}/usr/share/dbus-1/system-services/
cp -a doc/html %{buildroot}%{_docdir}/antidote/

%make_install

# >> install post
# << install post






%files
%defattr(-,root,root,-)
%{_bindir}/healthd
%config /etc/dbus-1/system.d/healthd.conf
%config /usr/share/dbus-1/system-services/com.signove.health.service
%{_libdir}/libantidote.*

%exclude %{_bindir}/ieee_manager
%exclude %{_bindir}/ieee_agent
%exclude %{_bindir}/simple_main
%exclude %{_bindir}/sample_bt_agent
%exclude %{_bindir}/ieee_manager_console
%exclude %{_bindir}/main_test_suite

%files devel
%defattr(-,root,root,-)
%{_includedir}/antidote/*
%{_libdir}/pkgconfig/antidote.pc
%{_libdir}/libbluezplugin*

%files devel-doc
%defattr(-,root,root,-)
%{_docdir}/antidote

# >> files
# << files

%changelog
* Mon Dec 20 2010 Elvis Pfutzenreuter <epx@signove.com> - 1.0.0
- Added devel-doc package

* Wed Dec 15 2010 Jose Luis do Nascimento <jose.nascimento@signove.com> - 1.0.0
- First release of antidote

