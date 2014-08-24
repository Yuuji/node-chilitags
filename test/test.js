var Chilitags = require('../build/Release/chilitags');
var fs = require('fs');

var chilitags = new Chilitags.Chilitags();

var filename = process.cwd() + '/image.png';

function detect() {
	var tags = chilitags.detect();
        fs.writeFileSync(filename, new Buffer(tags.image, 'base64'));
	for (var key in tags.tags) {
		var tag = tags.tags[key];
		console.log(tag.id + ': ' + tag.center.x + ' - ' + tag.center.y);
	}
	setTimeout(detect, 2000);
}

detect();
