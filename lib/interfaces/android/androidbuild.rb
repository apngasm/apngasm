#!/usr/bin/env ruby

require 'git'
require 'fileutils'

@build_dir = ARGV[0] || Dir.pwd
@source_dir = ARGV[1] || "#{@build_dir}/../../lib/"
@natives_dir = ARGV[2] || "#{@source_dir}/tmp/build/"
@interface_dir = "#{@build_dir}/japngasm/"
@platforms = [
  { :target => "arm", :arch => "armeabi"},
  { :target => "arm", :arch => "armeabi-v7a"},
  { :target => "arm64", :arch => "arm64-v8a"},
  { :target => "x86", :arch => "x86"},
  { :target => "x86_64", :arch => "x86_64"},
  { :target => "mips", :arch => "mips"},
]

puts "== Remove old files"
FileUtils.rm_rf("#{@build_dir}/natives")
FileUtils.rm_rf("#{@build_dir}/libs")
FileUtils.rm_rf("#{@build_dir}/jni")
FileUtils.rm_rf("#{@build_dir}/tmp")

puts "== Copying natives"
for platform in @platforms do
  src = "#{@natives_dir}/natives/lib/#{platform[:target]}"
  dest = "#{@build_dir}/natives/lib/#{platform[:target]}"
  if File.exists? src
    FileUtils.mkdir_p(dest)
    FileUtils.cp_r(Dir["#{src}/*.so"].collect{|f| File.expand_path(f)}, "#{dest}/", {remove_destination: true})
  end
end

puts "== Copying NDK build files and modifying variables"
FileUtils.cp_r("#{@source_dir}/interfaces/android/jni", @build_dir, {remove_destination: true})

txt = File.read("#{@build_dir}/jni/Android.mk")

# set source path
txt.gsub!("$(OR_INCLUDE_PATH)", "#{@source_dir}/src/ #{@build_dir}/src/ #{ENV["CRYSTAX_NDK"]}/sources/boost/1.58.0/include/ #{@natives_dir}/natives/include/")
txt.gsub!("$(OR_LIB_PATH)", "#{@build_dir}/natives/lib/")

cpp_files = Dir.glob("#{@source_dir}/src/**/*.cpp")
cpp_files_string = ""
cpp_files.each { |cpp_file| cpp_files_string += "#{cpp_file} " }
txt.gsub!("$(OR_CPP_SOURCES)", cpp_files_string )

File.open("#{@build_dir}/jni/Android.mk", "w") { |mkfile| mkfile.puts txt }

puts "== Creating Native Interface Java sources"
`swig -c++ -java -package japngasm -outdir #{@interface_dir} -o #{@build_dir}/jni/apng_wrap.cpp #{@source_dir}/src/apng.i`

puts "== Running NDK Build"
`NDK_PROJECT_PATH=#{@build_dir} #{ENV["CRYSTAX_NDK"]}/ndk-build`

puts "== Copying temporary natives for jar"
FileUtils.mkdir_p("#{@build_dir}/tmp/lib/")
for platform in @platforms do
  src_natives = "#{@build_dir}/natives/lib/#{platform[:target]}"
  src_libs = "#{@build_dir}/libs/#{platform[:arch]}"
  dest = "#{@build_dir}/tmp/lib/#{platform[:arch]}"

  if File.exists? src_libs
    FileUtils.mkdir_p(dest)
    FileUtils.cp_r(Dir.glob("#{src_natives}/*"), dest, {remove_destination: true})
    FileUtils.cp_r(Dir.glob("#{src_libs}/*"), dest, {remove_destination: true})
  end
end
