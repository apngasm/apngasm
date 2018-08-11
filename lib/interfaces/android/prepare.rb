#!/usr/bin/env ruby

require 'git'
require 'fileutils'

@build_dir = ARGV[0] || Dir.pwd

@build_api_level_32 = 16
@build_api_level_64 = 21
@build_targets = ["arm", "arm64", "x86", "x86_64"] #, "mips"]

puts "=== Preparing Android build dependencies"

def init()
  if @build_api_level_64 < 21
    @build_targets.reject!{|value| value.include?('64')}
  end

  FileUtils.mkdir_p("#{@build_dir}") unless File.exists? "#{@build_dir}"
  Dir.chdir(@build_dir)
  @build_dir = Dir.pwd
end

def chain_env(arch)
  api_level = arch.include?('64') ? @build_api_level_64 : @build_api_level_32
  case arch
  when "arm"
    return "CC=\"#{@build_dir}/toolchains/arm/bin/arm-linux-androideabi-gcc --sysroot=$CRYSTAX_NDK/platforms/android-#{api_level}/arch-arm\" LD=#{@build_dir}/toolchains/arm/bin/arm-linux-androideabi-ld AR=#{@build_dir}/toolchains/arm/bin/arm-linux-androideabi-ar RANLIB=#{@build_dir}/toolchains/arm/bin/arm-linux-androideabi-ranlib STRIP=#{@build_dir}/toolchains/arm/bin/arm-linux-androideabi-strip "
  when "x86"
    return "CC=\"#{@build_dir}/toolchains/x86/bin/i686-linux-android-gcc --sysroot=$CRYSTAX_NDK/platforms/android-#{api_level}/arch-x86\" LD=#{@build_dir}/toolchains/x86/bin/i686-linux-android-ld AR=#{@build_dir}/toolchains/x86/bin/i686-linux-android-ar RANLIB=#{@build_dir}/toolchains/x86/bin/i686-linux-android-ranlib STRIP=#{@build_dir}/toolchains/x86/bin/i686-linux-android-strip "
  when "mips"
    return "CC=\"#{@build_dir}/toolchains/mips/bin/mipsel-linux-android-gcc --sysroot=$CRYSTAX_NDK/platforms/android-#{api_level}/arch-mips\" LD=#{@build_dir}/toolchains/mips/bin/mipsel-linux-android-ld AR=#{@build_dir}/toolchains/mips/bin/mipsel-linux-android-ar RANLIB=#{@build_dir}/toolchains/mips/bin/mipsel-linux-android-ranlib STRIP=#{@build_dir}/toolchains/mips/bin/mipsel-linux-android-strip "
  when "arm64"
    return "CC=\"#{@build_dir}/toolchains/arm64/bin/aarch64-linux-android-gcc --sysroot=$CRYSTAX_NDK/platforms/android-#{api_level}/arch-arm64\" LD=#{@build_dir}/toolchains/arm64/bin/aarch64-linux-android-ld AR=#{@build_dir}/toolchains/arm64/bin/aarch64-linux-android-ar RANLIB=#{@build_dir}/toolchains/arm64/bin/aarch64-linux-android-ranlib STRIP=#{@build_dir}/toolchains/arm64/bin/aarch64-linux-android-strip "
  when "x86_64"
    return "CC=\"#{@build_dir}/toolchains/x86_64/bin/x86_64-linux-android-gcc --sysroot=$CRYSTAX_NDK/platforms/android-#{api_level}/arch-x86_64\" LD=#{@build_dir}/toolchains/x86_64/bin/x86_64-linux-android-ld AR=#{@build_dir}/toolchains/x86_64/bin/x86_64-linux-android-ar RANLIB=#{@build_dir}/toolchains/x86_64/bin/x86_64-linux-android-ranlib STRIP=#{@build_dir}/toolchains/x86_64/bin/x86_64-linux-android-strip "
  end
end

def setup_paths()
  # base
  Dir.mkdir("#{@build_dir}/natives") unless File.exists? "#{@build_dir}/natives"

  # include
  Dir.mkdir("#{@build_dir}/natives/include") unless File.exists? "#{@build_dir}/natives/include"
  @build_targets.each do |target|
    Dir.mkdir("#{@build_dir}/natives/include/#{target}") unless File.exists? "#{@build_dir}/natives/include/#{target}"
  end

  # lib
  Dir.mkdir("#{@build_dir}/natives/lib") unless File.exists? "#{@build_dir}/natives/lib"
  @build_targets.each do |target|
    Dir.mkdir("#{@build_dir}/natives/lib/#{target}") unless File.exists? "#{@build_dir}/natives/lib/#{target}"
  end

  # bin
  Dir.mkdir("#{@build_dir}/natives/bin") unless File.exists? "#{@build_dir}/natives/bin"
  @build_targets.each do |target|
    Dir.mkdir("#{@build_dir}/natives/bin/#{target}") unless File.exists? "#{@build_dir}/natives/bin/#{target}"
  end

  # jumper links
  @build_targets.each do |target|
    Dir.mkdir("#{@build_dir}/natives/#{target}") unless File.exists? "#{@build_dir}/natives/#{target}"
    FileUtils.ln_s("#{@build_dir}/natives/bin/#{target}", "#{@build_dir}/natives/#{target}/bin") unless File.exists? "#{@build_dir}/natives/#{target}/bin"
    FileUtils.ln_s("#{@build_dir}/natives/include/#{target}", "#{@build_dir}/natives/#{target}/include") unless File.exists? "#{@build_dir}/natives/#{target}/include"
    FileUtils.ln_s("#{@build_dir}/natives/lib/#{target}", "#{@build_dir}/natives/#{target}/lib") unless File.exists? "#{@build_dir}/natives/#{target}/lib"
  end
end

# Buld Chains
def prepare_chains()
  puts "== Preparing Android NDK Build Chains"
  @build_targets.each do |target|
    puts "= #{target}..."
    api_level = target.include?('64') ? @build_api_level_64 : @build_api_level_32
    `$CRYSTAX_NDK/build/tools/make-standalone-toolchain.sh --platform=android-#{api_level} --arch=#{target} --install-dir=#{@build_dir}/toolchains/#{target}` unless Dir.exists?("#{@build_dir}/toolchains/#{target}")
  end

  FileUtils.mkdir_p("#{@build_dir}/natives/lib")
  FileUtils.mkdir_p("#{@build_dir}/natives/include")
  FileUtils.mkdir_p("#{@build_dir}/natives")
  @build_targets.each do |target|
    FileUtils.mkdir_p("#{@build_dir}/natives/lib/#{target}")
  end
end

# libpng
def build_libpng()
  if Dir.exists? "#{@build_dir}/libpng"
    puts "libpng repository found. Updating..."
    Dir.chdir "#{@build_dir}/libpng"
    `git reset HEAD --hard`
    `git clean -fdx`
    puts 'Updated.'
  else
    puts 'libpng repository not found. Cloning...'
    Dir.chdir "#{@build_dir}"
    `git clone https://github.com/glennrp/libpng`
    puts 'Cloned.'
  end

  Dir.chdir "#{@build_dir}/libpng"
  `git checkout v1.6.25`

  puts '== Building libpng'

  if @build_targets.include? 'arm'
    puts '= Building for arm'
    `git clean -fdx`
    `autoreconf --force --install`
    `#{chain_env('arm')} ./configure --prefix=#{@build_dir}/natives/arm/ --disable-static --host=arm-linux-androideabi && make && make install`
  end

  if @build_targets.include? 'x86'
    puts '= Building for x86'
    `git clean -fdx`
    `autoreconf --force --install`
    `#{chain_env('x86')} ./configure --prefix=#{@build_dir}/natives/x86/ --disable-static --host=i686-linux-android && make && make install`
  end

  if @build_targets.include? 'mips'
    puts '= Building for mips'
    `git clean -fdx`
    `autoreconf --force --install`
    `#{chain_env('mips')} ./configure --prefix=#{@build_dir}/natives/mips/ --disable-static --host=mipsel-linux-android && make && make install`
  end

  if @build_targets.include? 'arm64'
    puts '= Building for arm64'
    `git clean -fdx`
    `autoreconf --force --install`
    `#{chain_env('arm64')} ./configure --prefix=#{@build_dir}/natives/arm64/ --disable-static --host=aarch64-linux-android && make && make install`
  end

  if @build_targets.include? 'x86_64'
    puts '= Building for x86_64'
    `git clean -fdx`
    `autoreconf --force --install`
    `#{chain_env('x86_64')} ./configure --prefix=#{@build_dir}/natives/x86_64/ --disable-static --host=x86_64-linux-android && make && make install`
  end

  Dir.chdir @build_dir
end

def check_lock()
  if File.exists? "#{@build_dir}/buildlock"
    puts '=> Build lock found. Skipping dependency preparation.'
    puts '=> *Delete the buildlock file to update and regenerate.'
    exit 0
  end
end

def check_env()
  if ENV['CRYSTAX_NDK'] != nil && ENV['CRYSTAX_NDK'] != ""
    puts "CRYSTAX_NDK is set to: #{ENV['CRYSTAX_NDK']}"
  else
    puts "CRYSTAX_NDK is not set! You must set the CRYSTAX_NDK environment variable."
    exit 1
  end
end

def set_lock()
  # Create lock and exit
  puts "Dependencies acquired. Generating build lock."
  lock_file = File.new("#{@build_dir}/buildlock", 'w')
  lock_file.puts "Delete this file to re-build Android build dependencies"
  lock_file.close
end

check_lock()

init()
check_env()
setup_paths()
prepare_chains()

build_libpng()

set_lock()

exit 0
