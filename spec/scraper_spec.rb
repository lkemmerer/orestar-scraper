require 'spec_helper'
require_relative '../lib/scraper.rb'

describe Scraper do
  let(:config) {
    {
      'orestar_url' => 'http://example.com',
      'download_location' => tmp_path,
      'date_format' => '%Y-%m-%d'
    }
  }
  let(:faked_index_page) { File.read(fixture_file('search_index.html')) }
  let(:faked_results_page) { File.read(fixture_file('search_results.html')) }

  subject { Scraper.new(config) }

  before :each do
    FileUtils.rm Dir.glob("#{tmp_path}/*.*")
    FakeWeb.register_uri(:get,
      'http://example.com',
      :body => faked_index_page,
      :content_type => 'text/html')
    FakeWeb.register_uri(:get,
      /gotoPublicTransactionSearchResults/,
      :body => faked_results_page,
      :content_type => 'text/html')
    FakeWeb.register_uri(:get,
      /XcelCNESearch/,
      :body => 'i_am_a_file')
  end


  it 'results in a file download' do
    subject.download_csv
    Dir.glob("#{tmp_path}/*.xls").size.should == 1
  end
end
