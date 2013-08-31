require 'fakeweb'
require 'ruby-debug'

RSpec.configure do |config|
  config.treat_symbols_as_metadata_keys_with_true_values = true
  config.run_all_when_everything_filtered = true
  config.filter_run :focus

  config.order = 'random'
  config.before :all do
    FileUtils.mkdir tmp_path unless File.exist? tmp_path
  end
end

def tmp_path
  File.join(File.dirname(__FILE__), 'tmp')
end

def fixture_path
  File.join(File.dirname(__FILE__), 'fixtures')
end

def fixture_file(file)
  File.join(fixture_path, file)
end
