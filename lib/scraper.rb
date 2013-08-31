require 'mechanize'

class Scraper
  IndexPage = {
    :search_form_name => 'cneSearchForm',
    :start_date_field => 'cneSearchTranStartDate',
    :end_date_field => 'cneSearchTranEndDate',
    :search_button_name => 'search',
  }

  ResultsPage = {
    :file_download_href => 'XcelCNESearch'
  }

  def initialize(config)
    @root_url          = config['orestar_url']
    @date_format       = config['date_format']
    @download_location = config['download_location']
    @agent = Mechanize.new
  end

  def download_csv(from_date=nil, to_date=Date.today)
    from_date ||= Date.today - 7
    @agent.get @root_url do |search_page|
      search_page.form_with(:name => IndexPage[:search_form_name]) do |form|
        form.send("#{IndexPage[:start_date_field]}=", from_date.strftime(@date_format))
        form.send("#{IndexPage[:end_date_field]}=", to_date.strftime(@date_format))
        search_button = form.button_with(:value => /#{IndexPage[:search_button]}/i)
        form.click_button search_button
      end
    end

    File.open("#{@download_location}/#{from_date.to_s}-#{to_date.to_s}.xls", 'w+') do |file|
      file << @agent.get_file(ResultsPage[:file_download_href])
    end
  end
end
