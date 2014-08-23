var Chilitags = require('../build/Release/chilitags');

var chilitags = new Chilitags.Chilitags();

function detect() {
	var tags = chilitags.detect();
	for (var key in tags.tags) {
		var tag = tags.tags[key];
		console.log(tag.id + ': ' + tag.center.x + ' - ' + tag.center.y);
	}

	setTimeout(detect, 1);
}

detect();
