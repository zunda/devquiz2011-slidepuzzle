#!/usr/bin/ruby
# usage: ruby solve.rb problems.txt
#
# to change settingws:
# irb> require 'drb/drb'
# irb> there = DRbObject.new_with_uri('druby://localhost:5000')
# irb> there.maxproc = 2
#
require 'drb/drb'
require './nfork'

command = './solver'
ddir = 'answers'
Dir.mkdir(ddir) unless File.directory?(ddir)

class Conf
	attr_accessor :ttl
	attr_accessor :maxproc

	def initialize(nproc = 1)
		@ttl = 256
		@maxproc = nproc
	end
end

class Board
	attr_reader :i, :str, :len
	def initialize(i, str)
		@i = i
		@str = str
		@len = str.split(/,/)[0..1].map{|e| e.to_i}.inject(&:*)
	end
end

conf = Conf.new
DRb.start_service('druby://localhost:5000', conf)

lx, rx, ux, dx = ARGF.gets.chomp.split(/ /).map{|e| e.to_i}
n = ARGF.gets.to_i

boards = Array.new
n.times do |i|
	boards << Board.new(i, ARGF.gets.chomp)
end
boards.sort!{|a, b| a.len <=> b.len}

boards.each do |board|
	dst = ddir + '/%04d.txt' % board.i
	Process.maxchild = conf.maxproc
	unless File.file?(dst)
		fork do
			$stderr.puts "#{board.i} #{board.str}"
			result = %x|#{command} -v -n #{conf.ttl} -p #{board.str}|
			unless result.empty?
				results = result.split(/\n/).sort
				File.open(dst, 'w') do |f|
					f.puts results.join("\n")
				end
				$stderr.puts "#{board.str} -> #{results[0]}"
			end
		end
	end
end
Process.waitall
