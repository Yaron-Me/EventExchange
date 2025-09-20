#include <SQLiteCpp/SQLiteCpp.h>
#include <print>
#include <iostream>
#include <map>

#include "../utility/uuid.hpp"
#include "utility.hpp"
#include "event.hpp"

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
            auto& db = getDatabase();
            
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
            auto& db = getDatabase();

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
                const auto issued{query.getColumn(3).getUInt()};
                const auto createdAt{query.getColumn(4).getString()};
                const auto shareIdStr{query.getColumn(5).getString()};
                const auto shareName{query.getColumn(6).getString()};
                const auto shareType{query.getColumn(7).getString()};

                auto [it, inserted] = eventsMap.try_emplace(eventIdStr, EventData{
                    .id = utility::stringToUUID(eventIdStr),
                    .name = eventName,
                    .description = description,
                    .issued = issued,
                    .createdAt = createdAt,
                    .yesShare = {},
                    .noShare = {}
                });

                if (shareType == "YES") {
                    it->second.yesShare = {utility::stringToUUID(shareIdStr), shareName};
                } else if (shareType == "NO") {
                    it->second.noShare = {utility::stringToUUID(shareIdStr), shareName};
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

    const EventData getEvent(const boost::uuids::uuid& eventId) {
        EventData eventData;

        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, R"(
                SELECT
                    e.id, e.name, e.description, e.created_at, e.issued,
                    sh.id, sh.name, sh.type
                FROM
                    events e
                JOIN
                    shares sh ON e.id = sh.event_id
                WHERE
                    e.id = ?;
            )"};

            query.bind(1, utility::uuidToString(eventId));

            while (query.executeStep()) {
                const auto eventIdStr{query.getColumn(0).getString()};
                const auto eventName{query.getColumn(1).getString()};
                const auto description{query.getColumn(2).getString()};
                const auto createdAt{query.getColumn(3).getString()};
                const auto issued{query.getColumn(4).getUInt()};
                const auto shareIdStr{query.getColumn(5).getString()};
                const auto shareName{query.getColumn(6).getString()};
                const auto shareType{query.getColumn(7).getString()};

                eventData.id = utility::stringToUUID(eventIdStr);
                eventData.name = eventName;
                eventData.description = description;
                eventData.createdAt = createdAt;
                eventData.issued = issued;

                if (shareType == "YES") {
                    eventData.yesShare = {utility::stringToUUID(shareIdStr), shareName};
                } else if (shareType == "NO") {
                    eventData.noShare = {utility::stringToUUID(shareIdStr), shareName};
                }
            }
        } catch (const std::exception& e) {
            std::print(std::cerr, "Error fetching event: {}\n", e.what());
        }

        return eventData;
    }

    void updateIssuedShares(const boost::uuids::uuid& eventId, std::int64_t quantity) {
        try {
            auto& db = getDatabase();

            SQLite::Statement query{db, R"(
                UPDATE events
                SET issued = issued + ?
                WHERE id = ?;
            )"};

            query.bind(1, quantity);
            query.bind(2, utility::uuidToString(eventId));

            query.exec();
        } catch (const std::exception& e) {
            std::print(std::cerr, "Error updating issued shares: {}\n", e.what());
        }
    }
}