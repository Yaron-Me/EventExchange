#include <SQLiteCpp/SQLiteCpp.h>
#include <print>
#include <iostream>
#include <map>

#include "../utility/uuid.hpp"
#include "utility.hpp"
#include "exchange.hpp"

namespace database {
    std::tuple<bool, EventData> createEvent(const std::string& eventName,
                     const std::string& eventDescription, const std::string& yesShareName,
                     const std::string& noShareName) {
        const auto eventId{utility::generateUUID()};
        const auto yesShareId{utility::generateUUID()};
        const auto noShareId{utility::generateUUID()};

        const std::string eventIdStr{utility::uuidToString(eventId)};
        const std::string yesShareIdStr{utility::uuidToString(yesShareId)};
        const std::string noShareIdStr{utility::uuidToString(noShareId)};

        try {
            SQLite::Database db{getDatabasePath(), SQLite::OPEN_READWRITE};
            db.exec("PRAGMA foreign_keys = ON;");
            
            SQLite::Transaction transaction{db};
            
            SQLite::Statement eventQuery{db, "INSERT INTO events (id, name, description) VALUES (?, ?, ?)"};
            eventQuery.bind(1, eventIdStr);
            eventQuery.bind(2, eventName);
            eventQuery.bind(3, eventDescription);
            eventQuery.exec();
            
            SQLite::Statement yesShareQuery{db, "INSERT INTO shares (id, name, event_id, type) VALUES (?, ?, ?, 'YES')"};
            yesShareQuery.bind(1, yesShareIdStr);
            yesShareQuery.bind(2, yesShareName);
            yesShareQuery.bind(3, eventIdStr);
            yesShareQuery.exec();
            
            SQLite::Statement noShareQuery{db, "INSERT INTO shares (id, name, event_id, type) VALUES (?, ?, ?, 'NO')"};
            noShareQuery.bind(1, noShareIdStr);
            noShareQuery.bind(2, noShareName);
            noShareQuery.bind(3, eventIdStr);
            noShareQuery.exec();

            transaction.commit();
            
            // Create EventData struct to return
            EventData eventData{
                .id = eventId,
                .name = eventName,
                .description = eventDescription,
                .createdAt = "", // Will be set by database
                .yesShare = {yesShareId, yesShareName},
                .noShare = {noShareId, noShareName}
            };
            
            return {true, eventData};
            
        } catch (const std::exception& e) {
            std::print(std::cerr, "Error creating event: {}\n", e.what());
            return {false, EventData{}};
        }
    }

    const std::vector<EventData> getEvents() {
        std::map<std::string, EventData> eventsMap;

        try {
            SQLite::Database db{getDatabasePath(), SQLite::OPEN_READONLY};

            std::print("{}", getDatabasePath());

            SQLite::Statement query{db, R"(
                SELECT
                    e.id, e.name, e.description, e.created_at,
                    sh.id, sh.name, sh.type
                FROM
                    events e
                JOIN
                    shares sh ON e.id = sh.event_id
                WHERE
                    e.settled = 0
                ORDER BY
                    e.created_at DESC;
            )"};

            while (query.executeStep()) {
                const auto eventIdStr{query.getColumn(0).getString()};
                const auto eventName{query.getColumn(1).getString()};
                const auto description{query.getColumn(2).getString()};
                const auto createdAt{query.getColumn(3).getString()};
                const auto shareIdStr{query.getColumn(4).getString()};
                const auto shareName{query.getColumn(5).getString()};
                const auto shareType{query.getColumn(6).getString()};

                if (eventsMap.find(eventIdStr) == eventsMap.end()) {
                    eventsMap[eventIdStr].id = utility::stringToUUID(eventIdStr);
                    eventsMap[eventIdStr].name = eventName;
                    eventsMap[eventIdStr].description = description;
                    eventsMap[eventIdStr].createdAt = createdAt;
                }

                if (shareType == "YES") {
                    eventsMap[eventIdStr].yesShare = {utility::stringToUUID(shareIdStr), shareName};
                } else if (shareType == "NO") {
                    eventsMap[eventIdStr].noShare = {utility::stringToUUID(shareIdStr), shareName};
                }
            }
        } catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching events: {}\n", e.what());
        }

        std::vector<EventData> results;
        results.reserve(eventsMap.size());
        for (const auto& [id, event] : eventsMap) {
            results.push_back(event);
        }

        return results;
    }
}