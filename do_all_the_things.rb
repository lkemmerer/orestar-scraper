require 'yaml'
require_relative 'lib/scraper'

current_path = File.dirname(__FILE__)
config = YAML.load_file(File.join(current_path, '/config.yml'))
Scraper.new(config).download_csv
