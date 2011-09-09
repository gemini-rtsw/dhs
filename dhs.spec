%define _prefix __auto__
%define gemopt opt
%define name dhs
%define version __auto__
%define release __auto__
%define repository gemini

Summary: the dhs server
Name: %{name}
Version: %{version}
Release: %{release}.%{dist}.%{repository}
URL: http://www.gemini.edu
Packager: Matthieu Bec <mbec@gemini.edu>, Vasu Upadhya <vupadhya@gemini.edu>
License: Gemini
Group: Gemini
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)
BuildArch: %{arch}
Prefix: %{_prefix}
Requires: gemini-top, gemini-setup, drama >= 1.5.2-9, dhsClient, xpa-tcl, cfitsio, dhs-config-server
BuildRequires: gemini-top, imake, byacc, drama-devel, dhsClient-devel, cfitsio-devel
BuildRequires: gemini-build
Source0: %{name}-%{version}.tar.gz

# stop rpm from stripping binaries (for debugging)
%define debug_package %{nil}
%define __strip /bin/true

%description
Gemini Data Handling System server(s).

%package devel
Summary: dhs
Group: Development/Gemini
Requires: dhs
%description devel
dhs

%package QlTools
Summary: dhs
Group: Development/Gemini
BuildRequires: gemini-build
Requires: ocswish drama >= 1.5.2-9 skycat xpa-tcl tclx qlplugins itk iwidgets cfitsio ds9
BuildRequires: imake, byacc, itk-devel, drama-devel, skycat-devel, dhsClient-devel, cfitsio-devel
%description QlTools
dhs

%package Console
Summary: dhs
Group: Development/Gemini
Requires: ocswish drama >= 1.5.2-9 xpa-tcl tclx
%description Console
dhs

%prep
%setup -q -n %name

%build
cd dhs
autoconf
./configure --prefix=$PWD/../release
gmake
gmake install
cd ..

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/tmp
mkdir -p $RPM_BUILD_ROOT/tmp
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/etc
mkdir -p $RPM_BUILD_ROOT/etc/init.d
cp -a dhs/release/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
rm -Rf $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/images
cp -a dhs/conf/dhs.conf* $RPM_BUILD_ROOT/%{_prefix}/tmp/
cp -a etc/dhs.profile.d $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d/dhs.sh
cp -a createDhsConfigDirs.sh $RPM_BUILD_ROOT/tmp/
cp -a dhs/conf/dhsconfig/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/
echo "%{_prefix}/%{gemopt}/dhs/lib" >  $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d/dhs.so.conf

cp -a dhs/conf/server.conf.* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/etc/

cp -a etc/dhs.init.d $RPM_BUILD_ROOT/etc/init.d/dhs
chmod 755 $RPM_BUILD_ROOT/etc/init.d/dhs
chmod 755 $RPM_BUILD_ROOT/tmp/createDhsConfigDirs.sh

%postun
/sbin/ldconfig
#if [ "$1" = "0" ] ; then  #last uninstall
#	userdel dhsuser
#fi

%post
/tmp/createDhsConfigDirs.sh %{version}

#%post QlTools
#/tmp/createDhsConfigDirs.sh %{version}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
# fixme: looks like there's more than we need listed here - cleanup
%{_prefix}/opt/%{name}/bin
%{_prefix}/opt/%{name}/lib/*.a
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/opt/%{name}/man
%{_prefix}/opt/%{name}/scripts/dhsCleanup
%{_prefix}/opt/%{name}/scripts/GemBootStart
%{_prefix}/opt/%{name}/scripts/MakeClassicalMedia
%{_prefix}/opt/%{name}/scripts/MakePrImages
%{_prefix}/opt/%{name}/scripts/MakeUserMedia
%{_prefix}/opt/%{name}/scripts/newsyslog
%{_prefix}/opt/%{name}/scripts/TestBootStart
%{_prefix}/opt/%{name}/config
%{_prefix}/opt/%{name}/sql
%{_prefix}/opt/%{name}/var
%{_prefix}/opt/%{name}/etc/
%{_prefix}/etc/profile.d
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp
/tmp/createDhsConfigDirs.sh
/etc/init.d/dhs

%files QlTools
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/scripts/dhsQlTool
%{_prefix}/opt/%{name}/bin/dhsQlServer
%{_prefix}/opt/%{name}/scripts/qlToolInst.sh
%{_prefix}/opt/%{name}/scripts/qlToolAltair.sh
%{_prefix}/opt/%{name}/scripts/qlToolWFS.sh
%{_prefix}/opt/%{name}/scripts/dhsQlToolDS9
%{_prefix}/opt/%{name}/scripts/GemBootStart
%{_prefix}/opt/%{name}/lib/*.a
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/opt/%{name}/lib/dhsQlTool*
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQls.config
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQlt.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsQls.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsQlt.config
%{_prefix}/opt/%{name}/etc/
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp
/tmp/createDhsConfigDirs.sh

%files Console
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/lib/*.a
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/opt/%{name}/lib/dhsConsole
%{_prefix}/opt/%{name}/scripts/dhsConsole
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsConsole.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsConsole.config
%{_prefix}/opt/%{name}/etc/
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp
/tmp/createDhsConfigDirs.sh

# fixme: doubt we'll ever need a dhs server development package
%files devel
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/include

%changelog
* Wed Sep 10 2008 Felix Kraemer <fkraemer@gemini.edu> 4.0
- seperated out QlTools and Console (ever needed??) in extra rpms
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 4.0
- removed EPICS dependencies
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 3.0
- added sample config that will work for a hostname called localhost
* Mon Jun 16 2008 Vasu Upadhya<vupadhya@gemini.edu> 2.0
- second rpm
* Wed Apr 2 2008 Matthieu Bec <mbec@gemini.edu> 1.0
- first rpm, removed SYBASE dependencies
