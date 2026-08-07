# we are in Resources/Deployment/macOS/ -> change directory to project root
cd ../../../

# set convenience variables
JUCEDir=submodules/JUCE
ProjucerPath="$JUCEDir"/extras/Projucer/Builds/MacOSX
ProjucerBinPath="$ProjucerPath"/build/Release/Projucer.app/Contents/MacOS/Projucer
JucerProjectPath=PipeDreamer.jucer
XCodeProjectPath=Builds/MacOSX/PipeDreamer.xcodeproj

# build projucer
xcodebuild -project "$ProjucerPath"/Projucer.xcodeproj -configuration Release -jobs 8

# export projucer project
"$ProjucerBinPath" --resave "$JucerProjectPath"

# start building the project. Uses -allowProvisioningUpdates since no fixed provisioning
# profile is pinned here (unlike a signed-for-distribution build); replace with an explicit
# PROVISIONING_PROFILE_SPECIFIER once a dedicated profile exists for this app.
xcodebuild -project "$XCodeProjectPath" -configuration Release -jobs 8 -allowProvisioningUpdates
