    #include <sstream>
    #include <fstream>
    #include <iostream>
    #include <cassert>
    #include <cmath>
    #include <stdexcept>
    #include "geometry.h"
    #include "xmlparser.h"
    #include "track.h"

    using namespace GPS;

    // Note: The implementation should exploit the relationship:
    //   totalTime() == restingTime() + travellingTime()

    seconds Track::totalTime() const
    {
        const bool implemented = false;
        assert(implemented);
    }

    seconds Track::restingTime() const
    {
        const bool implemented = false;
        assert(implemented);
    }

    seconds Track::travellingTime() const
    {
        const bool implemented = false;
        assert(implemented);
    }

    speed Track::maxSpeed() const
    {
        const bool implemented = false;
        assert(implemented);
    }

    speed Track::averageSpeed(bool includeRests) const
    {
        const bool implemented = false;
        assert(implemented);
    }

   //N0687265
     speed Track::maxRateOfAscent() const
    {
        const bool implemented = true;
        assert(implemented);
        seconds time;
        metres elevation;
        speed rate, topRate = 0;
        for(unsigned long int i = 1; i < positions.size(); ++i){
            time = arrived[i] - departed[i-1];
            elevation = positions[i].elevation() - positions[i-1].elevation();
            rate = elevation / time;
            if(rate > topRate){
                topRate = rate;
            }
        }

        return topRate;
    }

    speed Track::maxRateOfDescent() const
    {
        const bool implemented = false;
        assert(implemented);
    }

    Track::Track(std::string source, bool isFileName, metres granularity)
    {
        using namespace std;
        using namespace XML::Parser;
        string mergedTrkSegs,trkseg,lat,lon,ele,name,time,temp,temp2;
        metres horizontalDiff,verticalDiff;
        seconds startTime, currentTime, timeElapsed;
        ostringstream oss,oss2;
        unsigned int num = 0;
        this->granularity = granularity;
        if (isFileName)
        {
            ifstream fs(source);
            if (! fs.good()) throw invalid_argument("Error opening source file '" + source + "'.");
            oss << "Source file '" << source << "' opened okay." << endl;
            while (fs.good())
            {
                getline(fs, temp);
                oss2 << temp << endl;
            }
            source = oss2.str();
        }
            XML::Parser::pullElement (source, "gpx");
            XML::Parser::pullElement (source, "trk");
        if (elementExists(source, "name"))
        {
            temp = getAndEraseElement(source, "name");
            routeName = getElementContent(temp);
            oss << "Track name is: " << routeName << endl;
        }
        while (elementExists(source, "trkseg"))
        {
            temp = getAndEraseElement(source, "trkseg");
            trkseg = getElementContent(temp);
            getAndEraseElement(trkseg, "name");
            mergedTrkSegs += trkseg;
        }
        if (! mergedTrkSegs.empty()) source = mergedTrkSegs;
        if (! elementExists(source,"trkpt")) throw domain_error("No 'trkpt' element.");
        setLatLon(temp, source, lat, lon);
        if (elementExists(temp, "ele"))
        {
            temp2 = getElement(temp, "ele");
            ele = getElementContent(temp2);
            Position startPos = Position(lat,lon,ele);
            positions.push_back(startPos);
            oss << "Start position added: " << startPos.toString() << endl;
            ++num;
        }
        else
        {
            Position startPos = Position(lat,lon);
            positions.push_back(startPos);
            oss << "Start position added: " << startPos.toString() << endl;
            ++num;
        }

        if (elementExists(temp,"name"))
        {
            setElement (temp, temp2, name, "name");
        }
        pushBack (name, 0);
        if (! elementExists(temp,"time")) throw domain_error("No 'time' element.");
        setElement (temp, temp2, time, "time");
        startTime = currentTime = stringToTime(time);
        Position prevPos = positions.back(), nextPos = positions.back();
        while (elementExists(source, "trkpt"))

        {
            setLatLon(temp, source, lat, lon);
            if (elementExists(temp, "ele"))
            {
                setElement (temp, temp2, ele, "ele");
                nextPos = Position(lat,lon,ele);
            }
            else nextPos = Position(lat,lon);

            if (! elementExists(temp,"time")) throw domain_error("No 'time' element.");
            temp2 = getElement(temp,"time");
            time = getElementContent(temp2);
            currentTime = stringToTime(time);

            if (areSameLocation(nextPos, prevPos))
            {
                // If we're still at the same location, then we haven't departed yet.
                departed.back() = currentTime - startTime;
                oss << "Position ignored: " << nextPos.toString() << endl;
            }

            else

            {

                if (elementExists(temp,"name"))
                {
                    setElement (temp, temp2, name, "name");
                }
                else name = "";

                positions.push_back(nextPos);
                timeElapsed = currentTime - startTime;
                pushBack (name, timeElapsed);

                oss << "Position added: " << nextPos.toString() << endl;
                oss << " at time: " << to_string(timeElapsed) << endl;

                ++num;
                prevPos = nextPos;
            }
        }

        oss << num << " positions added." << endl;
        routeLength = 0;
        for (unsigned int i = 1; i < num; ++i )
        {
            horizontalDiff = distanceBetween(positions[i-1], positions[i]);
            verticalDiff = positions[i-1].elevation() - positions[i].elevation();
            routeLength += sqrt(pow(horizontalDiff,2) + pow(verticalDiff,2));
        }
        report = oss.str();
    }

    void Track::setGranularity(metres granularity)
    {
        bool implemented = false;
        assert(implemented);
    }

    seconds Track::stringToTime(const std::string & timeStr)
    {
        return stoull(timeStr);
    }


    void Track::pushBack (std::string name, seconds time)
    {
        positionNames.push_back(name);
        arrived.push_back(time);
        departed.push_back(time);
    }

    void Track::setElement (std::string temp, std::string temp2, std::string elementName, std::string elementName2)
    {
        temp2 = XML::Parser::getElement(temp,elementName2);
        elementName = XML::Parser::getElementContent(temp2);
    }

    void Track::setLatLon (std::string temp, std::string source, std::string lat, std::string lon)
    {
        using namespace XML::Parser;
        temp = getAndEraseElement(source, "trkpt");
        lat = pullAttribute(temp, "lat");
        lon = pullAttribute(temp, "lon");
        temp = getElementContent(temp);
    }

