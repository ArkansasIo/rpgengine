/* This file is part of the ArcLight engine (GPL v2 or later), see LICENSE.html */

#include <string>

// url (syntax: ArcLight://username:password@host:port)
bool ParseArcLightUri(const std::string& uri, std::string& username, std::string& password, std::string& host, int& port);

// url (syntax: rapid://ba:stable)
bool ParseRapidUri(const std::string& uri, std::string& tag);

