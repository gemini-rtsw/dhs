%define _prefix __auto__
%define gemopt opt
%define name dhs
%define version 1.0
%define release 5
%define repository gemini

Summary: the dhs server
Name: %{name}
Version: %{version}
Release: %{release}.%{dist}.%{repository}
URL: http://www.gemini.edu
Packager: Matthieu Bec <mbec@gemini.edu>
License: Gemini
Group: Gemini
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)
BuildArch: %{arch}
Prefix: %{_prefix}
Requires: gemini-top, gemini-setup, drama, dhsClient, skycat, epics-base, dhsClient
BuildRequires: gemini-top, imake, byacc, drama-devel, skycat-devel, epics-base-devel, dhsClient-devel
Source0: %{name}-%{version}.tar.gz

%define debug_package %{nil}

%description
Gemini Data Handling System server(s).

%prep
%setup -q -n %name

%build
autoconf
./configure --prefix=$PWD/release
gmake


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
mkdir -p $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d
cp -a release/* $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}
cp -a scripts/GemBootStart $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/bin
cp -a dhs.profile.d $RPM_BUILD_ROOT/%{_prefix}/etc/profile.d/dhs.sh
cp -a sample-config $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
chmod -R 0777 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var
chmod 0666 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/imp_startup/*
chmod 0666 $RPM_BUILD_ROOT/%{_prefix}/opt/%{name}/var/default_config_dir/*

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
%{_prefix}/opt/%{name}
%{_prefix}/etc/profile.d

%changelog
* Wed Apr 2 2008 Matthieu Bec <mbec@gemini.edu> 1.0
- first rpm
