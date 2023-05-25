# Gesture-controlled-granular-synthesiser
## Please make sure you follow all the requirements below

1. Download the addon and install them to openFrameworks  
ofxATK: https://github.com/aaronaanderson/ofxATK  
ofxLeapmotion2: https://github.com/genekogan/ofxLeapMotion2  
ofxGui: It is an official addon from OF, just include it in the project generator  
Please make sure you follow the guide from the addon's github page, especially ofxLeapmotion2:  

IMPORTANT!

On OS X, you must have this in the Run Script Build Phase of your Xcode project:
cp -f ../../../addons/ofxLeapMotion2/libs/lib/osx/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/libLeap.dylib"; install_name_tool -change ./libLeap.dylib @executable_path/libLeap.dylib "$TARGET_BUILD_DIR/$PRODUCT_NAME.app/Contents/MacOS/$PRODUCT_NAME";  
If you don't have this you'll see an error in the console: dyld: Library not loaded: @loader_path/libLeap.dylib  

2. Open the GCGS project and find skewWindow in Synth.cpp.  
Uncomment the above line with triWindow, right click triWindow and click Jump to definition.  
After the triWindow function, paste this:  
  
//modified by Bing  
inline float skewWindow(float x, float skew=0.5f, float p=0.f) {  
        constexpr float eps0 = 1e-8;  
        constexpr float eps1 = 1.-eps0;  
        skew = skew<eps0 ? eps0 : skew>eps1 ? eps1 : skew;  
        if(x < skew) return x/skew;  
        else return (1.-x)/(1.-skew);  
    }  

Then comment out the triWindow line in Synth.cpp.  
Now you are ready to build!  
If you encounter an error: Thread 1: EXC_BAD_ACCESS  
Please ignore it, clean the build folder and rerun, it will work if retry a few times.  
