
#Set natives build dir
# TODO change this!
SET(NATIVES_BUILD_DIR ${PROJECT_SOURCE_DIR}/tmp/build/)

#Get required libs/build chain tools
EXECUTE_PROCESS(COMMAND ruby interfaces/android/prepare.rb ${NATIVES_BUILD_DIR}
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# TODO & TEMPFIX
# Running SWIG through CMAKE doesn't work because CMAKE SWIG is too tied to the base system.
# As a temporary fix the actual compilation is commented out and we're using a postbuild script
# to do the actual SWIG handling; passing SWIG generated interfaces through an android mk project.
ADD_CUSTOM_TARGET(android
  ruby interfaces/android/androidbuild.rb ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR} ${NATIVES_BUILD_DIR}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# Jar test
FIND_PACKAGE(Java REQUIRED COMPONENTS Runtime Development)
INCLUDE(UseJava)

SET(SWIG_ANDROID_JAR_TARGET android_jar)
SET(SWIG_ANDROID_JAR_NAME libapngasm-android)
SET(SWIG_ANDROID_JAR_FILENAME ${SWIG_ANDROID_JAR_NAME}-${APNGASM_LIB_VERSION}.jar)
SET(APNGASM_JAVA_FILES
  ${PROJECT_BINARY_DIR}/japngasm/APNG.java
  ${PROJECT_BINARY_DIR}/japngasm/APNGAsm.java
  ${PROJECT_BINARY_DIR}/japngasm/APNGFrame.java
  ${PROJECT_BINARY_DIR}/japngasm/rgb.java
  ${PROJECT_BINARY_DIR}/japngasm/rgba.java
  ${PROJECT_BINARY_DIR}/japngasm/SWIGTYPE_p_p_unsigned_char.java
  ${PROJECT_BINARY_DIR}/japngasm/SWIGTYPE_p_unsigned_char.java
  ${PROJECT_BINARY_DIR}/japngasm/APNGFrameVector.java
  ${PROJECT_BINARY_DIR}/japngasm/APNGJNI.java
  ${PROJECT_BINARY_DIR}/japngasm/NativeLibLoader.java
)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/interfaces/android/src/NativeLibLoader.java.in ${PROJECT_BINARY_DIR}/japngasm/NativeLibLoader.java @ONLY)

ADD_JAR(${SWIG_ANDROID_JAR_TARGET}
  SOURCES ${APNGASM_JAVA_FILES}
  VERSION ${APNGASM_LIB_VERSION}
  OUTPUT_NAME ${SWIG_ANDROID_JAR_NAME}
)
SET_TARGET_PROPERTIES(${SWIG_ANDROID_JAR_TARGET}
  PROPERTIES
  EXCLUDE_FROM_ALL TRUE
)

ADD_DEPENDENCIES(${SWIG_ANDROID_JAR_TARGET} android)

ADD_CUSTOM_COMMAND(TARGET ${SWIG_ANDROID_JAR_TARGET}
  POST_BUILD
  COMMAND ${JAVA_ARCHIVE} -uvf ${SWIG_ANDROID_JAR_FILENAME} -C tmp lib/
)
