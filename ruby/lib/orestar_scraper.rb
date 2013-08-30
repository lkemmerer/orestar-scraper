require 'yaml'
require 'mechanize'

class OrestarScraper
  def initialize(config)
    @root_url           = config['orestar_url']
    @search_form_name   = config['search_form_name']
    @start_date_field   = config['start_date_field']
    @end_date_field     = config['end_date_field']
    @date_format        = config['date_format']
    @file_download_href = config['file_download_href']

    @download_location = File.join(File.dirname(__FILE__), '../' + config['download_location'])
    @agent = Mechanize.new
  end

  def download_csv(from_date=nil, to_date=Date.today)
    from_date ||= Date.today - 7
    @agent.get @root_url do |search_page|
      search_page.form_with(:name => @search_form_name) do |form|
        form.send("#{@start_date_field}=", from_date.strftime(@date_format))
        form.send("#{@end_date_field}=", to_date.strftime(@date_format))
        search_button = form.button_with(:value => /#{@search_button_name}/i)
        form.click_button search_button
      end
    end

    File.open("#{@download_location}/#{from_date.to_s}-#{to_date.to_s}.xls", 'w+') do |file|
      file << @agent.get_file(@file_download_href)
    end
  end
end

current_path = File.dirname(__FILE__)
config = YAML.load_file(File.join(current_path, '../config.yml'))
OrestarScraper.new(config).download_csv
