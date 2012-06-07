%define _prefix __auto__
%define gemopt opt
%define name dhs
%define version 1.7
%define release 14
%define repository gemini

Summary: The DHS Server
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
Requires: gemini-top gemini-setup drama >= 1.6.4 dhsClient cfitsio dhs-config-server dhs-libs dhs-QlServer syslog
#These requirements are to force the same architecture for the packages
Requires: libtclxpa.so.1
#Requires: xpa-tcl
BuildRequires: gemini-top, imake, byacc, drama-devel >= 1.6.4, dhsClient-devel, cfitsio-devel
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
Requires: ocswish drama >= 1.5.2-9 skycat qlplugins itk iwidgets cfitsio ds9 dhs-libs dhs-QlServer
#These requirements are to force the same architecture for the packages
Requires: libtclx8.4.so libtclxpa.so.1
#Requires: tclx xpa-tcl
BuildRequires: imake, byacc, itk-devel, drama-devel, skycat-devel, dhsClient-devel, cfitsio-devel
%description QlTools
dhs

%package Console
Summary: dhs
Group: Development/Gemini
Requires: ocswish drama >= 1.5.2-9 dhs-libs
#These requirements are to force the same architecture for the packages
Requires: libtclx8.4.so libtclxpa.so.1
#Requires: tclx xpa-tcl
%description Console
dhs

%package libs
Summary: dhs
Group: Development/Gemini
BuildRequires: gemini-build
%description libs
DHS common libraries.

%package QlServer
Summary: dhs
Group: Development/Gemini
BuildRequires: gemini-build
Requires: gemini-top, gemini-setup, drama >= 1.5.2-9, dhsClient, cfitsio, dhs-libs
#These requirements are to force the same architecture for the packages
Requires: libtclxpa.so.1
#Requires: xpa-tcl
%description Console
%description QlServer
Quicklook Server.

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
exit 0

%post
#Determine which syslog to use
if service syslog status &>/dev/null ; then
    logservice=syslog
else
    if service rsyslog status &>/dev/null ; then
        logservice=rsyslog
    else
        echo "Cannot find log service." >&2
        exit 1
    fi
fi
echo "Using log service $logservice"

/tmp/createDhsConfigDirs.sh %{version}
if ! grep ^gemdhs /etc/passwd > /dev/null ; then
	if ! grep ^gemini /etc/group > /dev/null ; then
		groupadd -g 2000 gemini
	fi
	useradd -g gemini -u 5052 gemdhs
fi

if [ "$1" = "0" ] ; then
	/sbin/chkconfig netfs on
	service netfs status || service netfs start
fi

#Make sure staging folder exist
[ -d /staging ] ||  mkdir -p /staging
chmod 775 /staging
chown root:gemini /staging

#Register dhs service for autostart
/sbin/chkconfig --list dhs &>/dev/null  || /sbin/chkconfig --add dhs


logfolder=%{_prefix}/var/log/dhs

if ! [ -e $logfolder/dhs.log ] ; then
    [ -d $logfolder ] || mkdir -p $logfolder
    chmod 775 $logfolder
    chown root:gemini $logfolder
    touch $logfolder/dhs.log
fi
chmod a+r $logfolder/dhs.log


if ! grep "local0.*$logfolder/dhs.log" /etc/$logservice.conf > /dev/null ; then
    sed '/local0/d' /etc/$logservice.conf
    echo -e "\nlocal0.*\t\t\t\t\t\t$logfolder/dhs.log" >> /etc/$logservice.conf
    service $logservice restart
fi

#recreate logrotate configuration file
[ -e /etc/logrotate.d/dhs ] && rm -f /etc/logrotate.d/dhs
echo -e "$logfolder/dhs.log {" >> /etc/logrotate.d/dhs
echo -e "\tdaily" >> /etc/logrotate.d/dhs
echo -e "\trotate 7" >> /etc/logrotate.d/dhs
echo -e "\tcreate 0644 root root" >> /etc/logrotate.d/dhs
echo -e "\tsharedscripts" >> /etc/logrotate.d/dhs
echo -e "\tpostrotate" >> /etc/logrotate.d/dhs
echo -e "\t\t/sbin/service $logservice restart" >> /etc/logrotate.d/dhs
echo -e "\tendscript" >> /etc/logrotate.d/dhs
echo -e "}" >> /etc/logrotate.d/dhs

service dhs start
exit 0

#%post QlTools
#/tmp/createDhsConfigDirs.sh %{version}

%post libs
/sbin/ldconfig

%preun
if [ "$1" = "0" ] ; then  #last uninstall
	service dhs stop
	/sbin/chkconfig --del dhs
fi
exit 0

%postun libs
/sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
# fixme: looks like there's more than we need listed here - cleanup
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsCommand
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsData
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsPut
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsSim4Data
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsStatus
%{_prefix}/opt/%{name}/man
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsCleanup
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/GemBootStart
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakeClassicalMedia
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakePrImages
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/MakeUserMedia
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/newsyslog
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/TestBootStart
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerStart.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerStop.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsServerRestart.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/rescue
%{_prefix}/opt/%{name}/config
%{_prefix}/opt/%{name}/sql
%{_prefix}/opt/%{name}/var
%{_prefix}/opt/%{name}/etc/
%{_prefix}/etc/profile.d
%{_prefix}/tmp
/tmp/createDhsConfigDirs.sh
%attr(755, root, root) /etc/init.d/dhs

%files QlTools
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/scripts/dhsQlTool
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/qlToolInst.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/qlToolAltair.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/qlToolWFS.sh
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsQlToolDS9
%{_prefix}/opt/%{name}/lib/dhsQlTool*
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQls.config
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsQlt.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsQls.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsQlt.config

%files Console
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/lib/dhsConsole
%attr(755, root, root) %{_prefix}/opt/%{name}/scripts/dhsConsole
%{_prefix}/opt/%{name}/var/sample-config/default_config_dir/dhsConsole.config
%{_prefix}/opt/%{name}/var/ops-*/default_config_dir/dhsConsole.config

%files libs
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/lib/*.so
%{_prefix}/opt/%{name}/lib/*.tcl
%{_prefix}/etc/ld.so.conf.d

%files QlServer
%attr(755, root, root) %{_prefix}/opt/%{name}/bin/dhsQlServer


# fixme: doubt we'll ever need a dhs server development package
%files devel
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/include
%{_prefix}/opt/%{name}/lib/*.a

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
