%define _prefix __auto__
%define gemopt opt
%define name dhs
%define version 1.0
%define release 12
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
Requires: gemini-top, gemini-setup, drama, skycat, dhsClient
BuildRequires: gemini-top, imake, byacc, drama-devel, skycat-devel, dhsClient-devel
Source0: %{name}-%{version}.tar.gz

%define debug_package %{nil}

%description
Gemini Data Handling System server(s).

%package devel
Summary: dhs
Group: Development/Gemini
Requires: dhs
%description devel
dhs

%prep
%setup -q -n %name

%build
autoconf
./configure --prefix=$PWD/release
gmake install


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d
cp -a release/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
cp -a scripts/GemBootStart $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/bin
cp -a dhs.profile.d $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d/dhs.sh
cp -a sample-config $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
# FIXME:
#  sample-config should supersede what's below - appears harmless, 
# leaving it for now there's no executable in the var configuration 
# so 0666 would be better - careful with directories though,leaving 
# it for now
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
chmod -R 0777 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/imp_startup
chmod -R 0777 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/imp_startup
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/default_config_dir
chmod -R 0777 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/default_config_dir

%clean
rm -rf $RPM_BUILD_ROOT/opt/dhs

%files
%defattr(-,root,root,-)
# fixme: looks like there's more than we need listed here - cleanup
%{_prefix}/opt/%{name}/bin
%{_prefix}/opt/%{name}/lib
%{_prefix}/opt/%{name}/man
%{_prefix}/opt/%{name}/scripts
%{_prefix}/opt/%{name}/config
%{_prefix}/opt/%{name}/images
%{_prefix}/opt/%{name}/sql
%{_prefix}/opt/%{name}/var
%{_prefix}/etc/profile.d

# fixme: doubt we'll ever need a dhs server development package
%files devel
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}/include

%changelog
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 4.0
- removed EPICS dependencies
* Tue Jun 17 2008 Matthieu Bec <mbec@gemini.edu> 3.0
- added sample config that will work for a hostname called localhost
* Mon Jun 16 2008 Vasu Upadhya<vupadhya@gemini.edu> 2.0
- second rpm
* Wed Apr 2 2008 Matthieu Bec <mbec@gemini.edu> 1.0
- first rpm, removed SYBASE dependencies
