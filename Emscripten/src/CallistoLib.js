
mergeInto(LibraryManager.library, {

	jsBeginLoadingFile: function (filePathPtr, outFileDataPtr, outLoadingFileStatePtr) {

		var filePath = '/assets/' + Pointer_stringify(filePathPtr);

		console.log("Loading file: " + filePath);

		var request = new XMLHttpRequest();
		request.open("GET", filePath, true);
		request.responseType = "arraybuffer";

		request.onload = function (e) {
			if (request.status == 200) {
		  		var arrayBuffer = request.response;
		  		if (arrayBuffer) {
		    		var byteArray = new Uint8Array(arrayBuffer);
					var bufferPtr = Module._malloc(byteArray.byteLength);
					setValue(outFileDataPtr, bufferPtr, 'i32');
					setValue(outFileDataPtr + 4, byteArray.byteLength, 'i32');

			    	for (var i = 0; i < byteArray.byteLength; i++) {
			      		setValue(bufferPtr + i, byteArray[i], 'i8');
				    }

				    setValue(outLoadingFileStatePtr, 0xf002, 'i32');

				    console.log("Sucessfully loaded: " + filePath);
		  		}

		  	} else {

		  		setValue(outLoadingFileStatePtr, 0xf003, 'i32');

		  		console.log("Failed to load: " + filePath);
		  	}
		};

		request.send(null);
	},

	jsGetTimestampMillis: function (outPtr) {

		var millis = performance.now();
		setValue(outPtr, millis, 'double');
	}
});