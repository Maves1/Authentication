Name:     ru.mavesoft.authentication
Version:  0.1
Release:  1
Summary:  Authentication
License:  Apache 2.0
URL:      https://github.com/Maves1/Authentication
Source0:  %{name}-%{version}.tar.bz2

Requires:   sailfishsilica-qt5 >= 0.10.9
BuildRequires:  pkgconfig(auroraapp)
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Quick)
BuildRequires:  pkgconfig(Qt5Sensors)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  cmake

%description
Authentication

%prep
%autosetup

%build
%cmake -B build/
%make_build -C build/

%install
cd build
%make_install

%files
%{_bindir}/%{name}
