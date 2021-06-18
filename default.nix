let
	pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation {
	pname = "mydraw";
	version = "1.0";
	src = ./.;

	buildInputs = [
		pkgs.qt5.qtbase
		pkgs.cmake
	];
	nativeBuildInputs = [ pkgs.qt5.wrapQtAppsHook ];

	configurePhase = ''
		cmake .
	'';

	buildPhase = ''
		make -j
	'';

	installPhase = ''
		mkdir -p $out/bin
		mv mydraw $out/bin
	'';
}
