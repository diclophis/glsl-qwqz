#!/usr/bin/env ruby

require 'find'

dir_prefix = File.dirname(File.realpath(ARGV[0]))
puts "CACHE MANIFEST"
puts "# built: " + Time.now.to_s
#puts "NETWORK:"
puts "CACHE:"
puts "index.html"
puts "sink.js"
puts "raptor_island.js"
Find.find(ARGV[0]) do |path|
  if FileTest.directory?(path) # dont output it into the cache
    if File.basename(path)[0] == ?.
      Find.prune # Don't look any further into this directory.
    else
      next
    end
  else
    puts File.realpath(path).gsub(dir_prefix + "/", "")
  end
end

