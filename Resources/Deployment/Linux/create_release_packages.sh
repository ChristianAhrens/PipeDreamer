Cores=2
if ! [ -z "$1" ]
then
  Cores="$1"
fi

chmod +x build_PipeDreamer_RaspberryPIOS.sh
./build_PipeDreamer_RaspberryPIOS.sh $Cores

# we are in Resources/Deployment/Linux/ -> change directory to project root
cd ../../../

# set convenience variables
PipeDreamerBinaryPath=Builds/LinuxMakefile/build/PipeDreamer
ChangeLogPath=CHANGELOG.md
LicensePath=LICENSE
DependencyInstallScript=get_dependencies_RaspberryPIOS.sh
DependencyInstallScriptPath="Resources/Deployment/Linux/""$DependencyInstallScript"
PipeDreamerZipTargetPath=PipeDreamer.zip
PackageContentCollectionPath=PackageContentCollection

mkdir "$PackageContentCollectionPath"

# copy required assets into temp collection folder
cp "$DependencyInstallScriptPath" "$PackageContentCollectionPath/$DependencyInstallScript"
cp "$ChangeLogPath" "$PackageContentCollectionPath/$ChangeLogPath"
cp "$LicensePath" "$PackageContentCollectionPath/$LicensePath"
mv "$PipeDreamerBinaryPath" "$PackageContentCollectionPath/PipeDreamer"

# change directory into collection folder
cd "$PackageContentCollectionPath"

# PipeDreamer app release package
test -f "$PipeDreamerZipTargetPath" && rm "$PipeDreamerZipTargetPath"
zip "$PipeDreamerZipTargetPath" "PipeDreamer" "$ChangeLogPath" "$LicensePath" "$DependencyInstallScript"

# back to root for further package handling
cd ..
cp "$PackageContentCollectionPath/$PipeDreamerZipTargetPath" "$PipeDreamerZipTargetPath"

# cleanup collection folder
test -d "$PackageContentCollectionPath" && rm -R "$PackageContentCollectionPath"
