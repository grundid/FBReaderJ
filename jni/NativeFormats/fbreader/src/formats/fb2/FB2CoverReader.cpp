/*
 * Copyright (C) 2004-2012 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <ZLFile.h>
#include <ZLImage.h>
//#include <ZLBase64EncodedImage.h>

#include "FB2CoverReader.h"

#include "../../library/Book.h"

FB2CoverReader::FB2CoverReader(const ZLFile &file) : myFile(file) {
}

shared_ptr<ZLImage> FB2CoverReader::readCover() {
	myReadCoverPage = false;
	myImageReference.erase();
	readDocument(myFile);
	return myImage;
}

void FB2CoverReader::startElementHandler(int tag, const char **attributes) {
	switch (tag) {
		case _COVERPAGE:
			myReadCoverPage = true;
			break;
		case _IMAGE:
			if (myReadCoverPage) {
				const std::string hrefName = xlinkNamespace() + ":href";
				const char *ref = attributeValue(attributes, hrefName.c_str());
				if (ref != 0 && *ref == '#' && *(ref + 1) != '\0') {
					myImageReference = ref + 1;
				}
			}
			break;
		case _BINARY:
		{
			const char *id = attributeValue(attributes, "id");
			const char *contentType = attributeValue(attributes, "content-type");
			if (id != 0 && contentType != 0 && myImageReference == id) {
				//myImage = new ZLBase64EncodedImage(contentType);
			}
		}
	}
}

void FB2CoverReader::endElementHandler(int tag) {
	switch (tag) {
		case _COVERPAGE:
			myReadCoverPage = false;
			break;
		case _DESCRIPTION:
			if (myImageReference.empty()) {
				interrupt();
			}
			break;
		case _BINARY:
			if (!myImage.isNull()) {
				if (!myImageBuffer.empty()) {
					//((ZLBase64EncodedImage&)*myImage).addData(myImageBuffer);
					myImageBuffer.clear();
				} else {
					myImage = 0;
				}
				interrupt();
			}
			break;
	}
}

void FB2CoverReader::characterDataHandler(const char *text, size_t len) {
	if (len > 0 && !myImage.isNull()) {
		myImageBuffer.push_back(std::string(text, len));
	}
}
