var xhrRequest = function (url, type, callback) { 
  var xhr = new XMLHttpRequest();
  xhr.onload   = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};

function getVerse() {
  var url = 'http://labs.bible.org/api/?passage=votd&type=json';
  console.log("Inside Verse");
  xhrRequest(url, 'GET',function(responseText) {
    console.log("2");
    var json = JSON.parse(responseText);
    console.log("Finished parsing ");
     var verse = json[0].text;
    console.log("got verse" + verse);
     console.log("The verse is " + verse);
    var ref = json[0].bookname + " " + json[0].chapter + ":" + json[0].verse;
       console.log("got reference string");
    console.log("The reference is " + ref);
          // Assemble dictionary using our keys
      var dictionary = {
        "KEY_VERSE": verse,
        "KEY_PASSAGE": ref
      };

      // Send to Pebble
      Pebble.sendAppMessage(dictionary,
        function(e) {
          console.log("VOTD sent to Pebble successfully!");
        },
        function(e) {
          console.log("Error sending VOTD to Pebble!");
        }
      );
  });
}
  // Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log('PebbleKit JS ready!');
    getVerse();  
  }
);

//Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log('AppMessage received!');
    getVerse();  
  }                     
);