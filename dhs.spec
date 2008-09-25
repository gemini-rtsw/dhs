%define _prefix __auto__
%define gemopt opt
%define name dhs
%define version 1.0
%define release 34
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
Requires: gemini-top, gemini-setup, drama, dhsClient, xpa-tcl, cfitsio
Requires: gemini-runtime
BuildRequires: gemini-top, imake, byacc, drama-devel, dhsClient-devel, cfitsio-devel
BuildRequires: gemini-build
Source0: %{name}-%{version}.tar.gz

%define debug_package

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
Requires: gemini-runtime
BuildRequires: gemini-build
Requires: ocswish drama skycat xpa-tcl qlplugins itk iwidgets cfitsio
BuildRequires: imake, byacc, itk-devel, drama-devel, skycat-devel, dhsClient-devel, qlplugins, cfitsio-devel
%description QlTools
dhs

%package Console
Summary: dhs
Group: Development/Gemini
Requires: gemini-runtime
Requires: ocswish drama
%description Console
dhs

%prep
%setup -q -n %name

%build
autoconf
./configure --prefix=$PWD/release
gmake
gmake install


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/tmp
cp -a release/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
cp -a conf/dhs.conf* $RPM_BUILD_ROOT/%{_prefix}/tmp/
#cp -a scripts/GemBootStart $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/bin
cp -a dhs.profile.d $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d/dhs.sh
cp -a sample-config $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/
echo "%{_prefix}/%{gemopt}/dhs/lib" >  $RPM_BUILD_ROOT/%{_prefix}/etc/ld.so.conf.d/dhs.so.conf


%postun
/sbin/ldconfig

%post
/sbin/ldconfig

#chmod  0666 %{_prefix}/opt/%{name}/var/sample-config/imp_startup/*
## for now until we have a group gemsoft where these files could belong to
#chmod  0666 %{_prefix}/opt/%{name}/var/sample-config/default_config_dir/*
cd %{_prefix}/opt/%{name}/var
if [ ! -d local-config-%{version} ]; then
	cp -a sample-config local-config-%{version}
	ln -snf local-config-%{version} local-config
fi
rm -rf auto-config
cp -a sample-config auto-config

chmod -R  777 %{_prefix}/opt/%{name}/var

## create auto configuration based on dhs.conf.$GEMINI_SITE
if [ "`/sbin/ifconfig | egrep 'addr:10\.'`" != "" ]; then
    export GEMINI_SITE=MK
else
    export GEMINI_SITE=CP
fi
sed -e '/#.*/ d' -e '/^$/ d' < %{_prefix}/tmp/dhs.conf.${GEMINI_SITE} > %{_prefix}/tmp/dhs.conf.tmp
cd %{_prefix}/opt/%{name}/var/sample-config/default_config_dir
echo sed \\ > %{_prefix}/tmp/sedscript.tmp
awk '{printf "-e \"s/%s/%s/g\" \\\n", $1,$2}' %{_prefix}/tmp/dhs.conf.tmp >> %{_prefix}/tmp/sedscript.tmp
chmod 755 %{_prefix}/tmp/sedscript.tmp
for i in `ls`; do 
%{_prefix}/tmp/sedscript.tmp < $i > %{_prefix}/opt/%{name}/var/auto-config/default_config_dir/$i
done
rm -f %{_prefix}/tmp/sedscript.tmp 
rm -f %{_prefix}/tmp/dhs.conf*
#for i in `ls`; do 
#sed \
#-e 's/STORE_HOST/dhsstorage/g' \
#-e 's/OLDP_HOST/dhsoldp/g' \
#-e 's/STA_HOST/dhsstatus/g' \
#-e 's/QLS_HOST/dhsqls/g' \
#-e 's/CMD_HOST/dhscmd/g' \
#-e 's/TOOL1_HOST/dhstool1/g' \
#-e 's/OCS_HOST/dhsocs/g' \
#-e 's/SIM_HOST/dhssad/g' \
#-e 's/DTS_HOST/dhsdtsremote/g' \
#< $i > ../../auto-config/default_config_dir/$i
#done

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
%{_prefix}/opt/%{name}/images
%{_prefix}/opt/%{name}/sql
%{_prefix}/opt/%{name}/var
%{_prefix}/etc/profile.d
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp

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
%{_prefix}/opt/%{name}/var/sample-config/imp_startup
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQls.config
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQlt.config
%{_prefix}/etc/profile.d
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp

%files Console
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/lib/*.a
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/opt/%{name}/lib/dhsConsole
%{_prefix}/opt/%{name}/var/sample-config/imp_startup
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsConsole.config
%{_prefix}/etc/profile.d
%{_prefix}/etc/ld.so.conf.d
%{_prefix}/tmp

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
