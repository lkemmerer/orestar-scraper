var request = require('request'),
    fs      = require('fs'),
    Cromag  = require('cromag'),
    OrestarImporter = require('./orestar_importer.js');

var base_uri = "https://secure.sos.state.or.us/orestar/";
var request_uri = function(startDate) {
  var now = new Cromag();
  if (startDate === null) { startDate = now.addDays(-7); }

  return base_uri + "gotoPublicTransactionSearchResults.do?" +
    "cneSearchButtonName=search&cneSearchFilerCommitteeId=*&cneSearchTranEndDate=" +
    now.toFormat("MM/DD/YYYY") + "&cneSearchTranStartDate=" + startDate.toFormat("MM/DD/YYYY");
};

request({
  uri: request_uri(null),
}, function(error, response, body) {
  if (error || response.statusCode !== 200) {
    console.log("Unexpected response/Error: " + error);
  } else {
    var sourceFile = './downloads/orestar_data.csv';
    var writeFile = fs.createWriteStream(sourceFile);

    request({
      uri: base_uri + "XcelCNESearch",
      headers: { cookie: response.headers['set-cookie'] }
    }, function(e, res, b) {
      if (e) {
        console.log(e);
      } else if (res.headers['content-type'] !== 'application/vnd.ms-excel') {
        console.log("Incorrect file type detected. " + res.headers['content-type']);
      }
    }).pipe(writeFile);

    writeFile.on('close', function() {
      // TODO: store data in database
      OrestarImporter(this.path);

      var archivedFile = './downloads/archived_csv/data-' + Date.now().toString() + '.csv';
      fs.rename(this.path, archivedFile, function(e) { if (e) { console.log(e); } });
      // TODO: clean this up if we get more than x files
    });
  }
});
