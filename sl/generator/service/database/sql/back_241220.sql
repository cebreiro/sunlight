-- MySQL dump 10.13  Distrib 8.0.18, for Win64 (x86_64)
--
-- Host: localhost    Database: cebreiro_sl
-- ------------------------------------------------------
-- Server version	8.0.18

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!50503 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `accounts`
--

DROP TABLE IF EXISTS `accounts`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `accounts` (
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `account` varchar(30) NOT NULL,
  `password` varchar(256) NOT NULL,
  `gm_level` tinyint(4) NOT NULL DEFAULT '0',
  `banned` tinyint(4) NOT NULL DEFAULT '0',
  `deleted` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `accounts_account_UNIQUE` (`account`) /*!80000 INVISIBLE */
) ENGINE=InnoDB AUTO_INCREMENT=35 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `accounts_storage_data`
--

DROP TABLE IF EXISTS `accounts_storage_data`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `accounts_storage_data` (
  `aid` bigint(20) NOT NULL,
  `page` tinyint(4) NOT NULL DEFAULT '1',
  `gold` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`aid`),
  CONSTRAINT `accounts_storage_data_aid_fk` FOREIGN KEY (`aid`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `accounts_storage_item`
--

DROP TABLE IF EXISTS `accounts_storage_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `accounts_storage_item` (
  `id` bigint(20) NOT NULL,
  `aid` bigint(20) NOT NULL,
  `item_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL,
  `page` tinyint(4) NOT NULL,
  `x` tinyint(4) NOT NULL,
  `y` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `accounts_storage_item_aid_fk` (`aid`),
  CONSTRAINT `accounts_storage_item_aid_fk` FOREIGN KEY (`aid`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters`
--

DROP TABLE IF EXISTS `characters`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters` (
  `id` bigint(20) NOT NULL,
  `aid` bigint(20) NOT NULL,
  `sid` tinyint(4) NOT NULL,
  `deleted` tinyint(4) NOT NULL DEFAULT '0',
  `slot` tinyint(4) NOT NULL,
  `name` varchar(60) NOT NULL,
  `hair` int(11) NOT NULL,
  `hair_color` int(11) NOT NULL,
  `face` int(11) NOT NULL,
  `skin_color` int(11) NOT NULL,
  `arms` tinyint(4) NOT NULL DEFAULT '1',
  `running` tinyint(4) NOT NULL DEFAULT '1',
  `dead` tinyint(4) NOT NULL DEFAULT '0',
  `gold` int(11) NOT NULL DEFAULT '0',
  `inventory_page` tinyint(4) NOT NULL DEFAULT '1',
  `zone` int(11) NOT NULL,
  `stage` int(11) NOT NULL DEFAULT '10000',
  `x` float NOT NULL,
  `y` float NOT NULL,
  `yaw` float NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  KEY `characters_aid_fk` (`aid`),
  CONSTRAINT `characters_aid_fk` FOREIGN KEY (`aid`) REFERENCES `accounts` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_item`
--

DROP TABLE IF EXISTS `characters_item`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_item` (
  `id` bigint(20) NOT NULL,
  `cid` bigint(20) NOT NULL,
  `item_id` int(11) NOT NULL,
  `quantity` int(11) NOT NULL,
  `pos_type` enum('inventory','equipment','quick_slot','pick','vendor','mix') NOT NULL,
  `page` tinyint(4) NOT NULL,
  `x` tinyint(4) NOT NULL,
  `y` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `characters_item_cid_fk_idx` (`cid`),
  CONSTRAINT `characters_item_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_job`
--

DROP TABLE IF EXISTS `characters_job`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_job` (
  `cid` bigint(20) NOT NULL,
  `id` int(11) NOT NULL,
  `type` int(11) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '1',
  `exp` int(11) NOT NULL DEFAULT '0',
  `skill_point` int(11) NOT NULL DEFAULT '0',
  KEY `characters_job_cid_fk_idx` (`cid`),
  CONSTRAINT `characters_job_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_profile_introduction`
--

DROP TABLE IF EXISTS `characters_profile_introduction`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_profile_introduction` (
  `cid` bigint(20) NOT NULL,
  `age` varchar(256) NOT NULL,
  `sex` varchar(256) NOT NULL,
  `mail` varchar(256) NOT NULL,
  `message` varchar(2048) NOT NULL,
  PRIMARY KEY (`cid`),
  CONSTRAINT `characters_profile_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_profile_setting`
--

DROP TABLE IF EXISTS `characters_profile_setting`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_profile_setting` (
  `cid` bigint(20) NOT NULL,
  `refuse_party_invite` tinyint(4) NOT NULL DEFAULT '0',
  `refuse_channel_invite` tinyint(4) NOT NULL DEFAULT '0',
  `refuse_guild_invite` tinyint(4) NOT NULL DEFAULT '0',
  `private` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`cid`),
  CONSTRAINT `characters_profile_setting_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_quest`
--

DROP TABLE IF EXISTS `characters_quest`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_quest` (
  `cid` bigint(20) NOT NULL,
  `id` int(11) NOT NULL,
  `state` int(11) NOT NULL,
  `flags` varchar(200) NOT NULL,
  `data` varchar(200) NOT NULL,
  KEY `characters_quest_cid_fk_idx` (`cid`) /*!80000 INVISIBLE */,
  KEY `characters_quest_cid_id_idx` (`cid`,`id`),
  CONSTRAINT `characters_quest_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_skill`
--

DROP TABLE IF EXISTS `characters_skill`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_skill` (
  `cid` bigint(20) NOT NULL,
  `id` int(11) NOT NULL,
  `job` int(11) NOT NULL,
  `level` int(11) NOT NULL,
  `cooldown` int(11) NOT NULL DEFAULT '0',
  `page` tinyint(4) NOT NULL DEFAULT '-1',
  `x` tinyint(4) NOT NULL DEFAULT '-1',
  `y` tinyint(4) NOT NULL DEFAULT '-1',
  `exp` int(11) NOT NULL DEFAULT '0',
  KEY `characters_skill_cid_fk_idx` (`cid`) /*!80000 INVISIBLE */,
  KEY `characters_skill_id_idx` (`id`),
  CONSTRAINT `characters_skill_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `characters_stat`
--

DROP TABLE IF EXISTS `characters_stat`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!50503 SET character_set_client = utf8mb4 */;
CREATE TABLE `characters_stat` (
  `cid` bigint(20) NOT NULL,
  `gender` tinyint(4) NOT NULL,
  `hp` int(11) DEFAULT NULL,
  `sp` int(11) DEFAULT NULL,
  `chr_lv` int(11) NOT NULL DEFAULT '1',
  `chr_exp` int(11) NOT NULL DEFAULT '0',
  `stat_point` int(11) NOT NULL DEFAULT '0',
  `str` int(11) NOT NULL,
  `dex` int(11) NOT NULL,
  `accr` int(11) NOT NULL,
  `health` int(11) NOT NULL,
  `intell` int(11) NOT NULL,
  `wis` int(11) NOT NULL,
  `will` int(11) NOT NULL,
  `water` tinyint(4) NOT NULL,
  `fire` tinyint(4) NOT NULL,
  `lightning` tinyint(4) NOT NULL,
  KEY `character_stat_cid_fk` (`cid`),
  CONSTRAINT `character_stat_cid_fk` FOREIGN KEY (`cid`) REFERENCES `characters` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping routines for database 'cebreiro_sl'
--
/*!50003 DROP PROCEDURE IF EXISTS `account_add` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `account_add`(
	IN account VARCHAR(20),
    IN password VARCHAR(256)
)
BEGIN
	INSERT INTO accounts 
		(account, password)
	VALUES 
		(account, password);

	SELECT 
		`id`,
        `account`,
        `password`,
		`gm_level`,
        `banned`,
        `deleted`
	FROM
		accounts 
	WHERE 
		accounts.account = account;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `account_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `account_get`(
	IN account VARCHAR(20)
)
BEGIN
	SELECT 
		`id`,
        `account`,
        `password`,
		`gm_level`,
        `banned`,
        `deleted`
	FROM
		accounts 
	WHERE 
		accounts.account = account;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `account_password_change` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `account_password_change`(
	IN account VARCHAR(20),
    IN password VARCHAR(256)
)
BEGIN
	UPDATE accounts SET password = password WHERE accounts.account = account;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `account_storage_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `account_storage_get`(
	IN aid BIGINT
)
BEGIN
	DECLARE pageValue INT;
	DECLARE goldValue INT;

	SELECT page, gold INTO pageValue, goldValue FROM accounts_storage_data WHERE accounts_storage_data.aid = aid;
    
    if ROW_COUNT() = 0 THEN
		INSERT INTO accounts_storage_data 
			(`aid`, `page`, `gold`)
		VALUES
			(aid, 1, 0);
		
        SET pageValue = 1;
        SET goldValue = 0;

    END IF;
    
    SELECT pageValue, goldValue;
    
	SELECT id, item_id, quantity, page, x, y 
    FROM accounts_storage_item as asi
    WHERE asi.aid = aid;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `characters_name_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `characters_name_get`()
BEGIN
	SELECT name FROM characters;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_create` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_create`(
	IN jsonString JSON
)
BEGIN
	DECLARE cid BIGINT;
    DECLARE i INT;
    DECLARE jsonArray JSON;
    DECLARE size INT;
    DECLARE itemJson JSON;
    DECLARE skillJson JSON;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
		SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'A custom error occurred during the transaction.';
    END;
    
    START TRANSACTION;
    
    SET cid = JSON_EXTRACT(jsonString, '$.id');

    INSERT INTO characters
		(`id`, `aid`, `sid`, `slot`, `name`, `hair`, `hair_color`, `face`, `skin_color`, `zone`, `x`, `y`)
	VALUES (
		cid,
        JSON_EXTRACT(jsonString, '$.aid'),
        JSON_EXTRACT(jsonString, '$.sid'),
        JSON_EXTRACT(jsonString, '$.slot'),
        JSON_UNQUOTE(JSON_EXTRACT(jsonString, '$.name')),
        JSON_EXTRACT(jsonString, '$.hair'),
        JSON_EXTRACT(jsonString, '$.hairColor'),
        JSON_EXTRACT(jsonString, '$.face'),
        JSON_EXTRACT(jsonString, '$.skinColor'),
        JSON_EXTRACT(jsonString, '$.zone'),
        JSON_EXTRACT(jsonString, '$.x'),
        JSON_EXTRACT(jsonString, '$.y')
    );
    
    
    INSERT INTO characters_stat
		( `cid`, `gender`, `str`, `dex`, `accr`, `health`, `intell`, `wis`, `will`, `water`, `fire`, `lightning` )
	VALUES (
		cid,
        JSON_EXTRACT(jsonString, '$.gender'),
        JSON_EXTRACT(jsonString, '$.str'),
        JSON_EXTRACT(jsonString, '$.dex'),
        JSON_EXTRACT(jsonString, '$.accr'),
        JSON_EXTRACT(jsonString, '$.health'),
        JSON_EXTRACT(jsonString, '$.intell'),
        JSON_EXTRACT(jsonString, '$.wis'),
        JSON_EXTRACT(jsonString, '$.will'),
        JSON_EXTRACT(jsonString, '$.water'),
        JSON_EXTRACT(jsonString, '$.fire'),
        JSON_EXTRACT(jsonString, '$.lightning')
    );
    
    INSERT INTO characters_job
		( `cid`, `id` )
	VALUES (
		cid,
        JSON_EXTRACT(jsonString, '$.job')
    );
    
    SET i = 0;
    SET jsonArray = JSON_EXTRACT(jsonString, '$.items');
    SET size = JSON_LENGTH(jsonArray);

    WHILE i < size DO 
		SET itemJson = JSON_EXTRACT(jsonArray, CONCAT('$[', i, ']'));
		
        INSERT INTO characters_item
			(`id`, `cid`, `item_id`, `quantity`, `pos_type`, `page`, `x`, `y`)
		VALUES (
			JSON_EXTRACT(itemJson, '$.id'),
			JSON_EXTRACT(itemJson, '$.cid'),
			JSON_EXTRACT(itemJson, '$.itemId'),
			JSON_EXTRACT(itemJson, '$.quantity'),
            JSON_UNQUOTE(JSON_EXTRACT(itemJson, '$.posType')),
			JSON_EXTRACT(itemJson, '$.page'),
			JSON_EXTRACT(itemJson, '$.x'),
			JSON_EXTRACT(itemJson, '$.y')
        );
    
		SET i = i + 1;
        
    END WHILE;
    
	SET i = 0;
    SET jsonArray = JSON_EXTRACT(jsonString, '$.skills');
    SET size = JSON_LENGTH(jsonArray);
    
    WHILE i < size DO 
		SET skillJson = JSON_EXTRACT(jsonArray, CONCAT('$[', i, ']'));
        
        INSERT INTO characters_skill
			( `cid`, `id`, `job`, `level` )
		VALUES (
			JSON_EXTRACT(skillJson, '$.cid'),
			JSON_EXTRACT(skillJson, '$.id'),
			JSON_EXTRACT(skillJson, '$.job'),
			JSON_EXTRACT(skillJson, '$.level')
        );
    
		SET i = i + 1;
        
    END WHILE;
    
    INSERT INTO characters_profile_setting (`cid`) VALUES (cid);

    COMMIT;
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_delete_soft` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_delete_soft`(
	IN cid BIGINT
)
BEGIN

	UPDATE characters
    SET deleted = 1
    WHERE id = cid;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_exp_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_exp_set`(
	IN cid BIGINT,
    IN exp INT
)
BEGIN
	UPDATE characters_stat SET chr_exp = exp WHERE characters_stat.cid = cid;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
    		
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_get`(
	IN cid BIGINT
)
BEGIN

	SELECT id, aid, name, hair, hair_color, face, skin_color, arms, running, dead, gold,inventory_page,zone, stage, x, y, yaw
    FROM characters as c
    WHERE c.id = cid;
    
    SELECT gender, hp, sp, chr_lv, chr_exp, stat_point, str, dex, accr, health, intell, wis, will, water, fire, lightning 
    FROM characters_stat as cs
    WHERE cs.cid = cid;
    
    SELECT id, type, level, exp, skill_point 
    FROM characters_job as cj
    WHERE cj.cid = cid;
    
    SELECT id, job, level, cooldown, page, x, y, exp
    FROM characters_skill as cs
    WHERE cs.cid = cid;
    
	SELECT id, item_id, quantity, pos_type, page, x, y
    FROM characters_item as ci
    WHERE ci.cid = cid;
    
    SELECT id, state, flags, data
    FROM characters_quest as cq
    WHERE cq.cid = cid;
    
    SELECT refuse_party_invite, refuse_channel_invite, refuse_guild_invite, private
    FROM characters_profile_setting as cps
    WHERE cps.cid = cid;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_hair_color_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_hair_color_set`(
	IN cid BIGINT,
    IN hairColor INT
)
BEGIN
	UPDATE characters SET hair_color = hairColor WHERE characters.id = cid;

	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_hair_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_hair_set`(
	IN cid BIGINT,
    IN hair INT
)
BEGIN
	UPDATE characters SET hair = hair WHERE characters.id = cid;

	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_job_add` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_job_add`(
	IN cid BIGINT,
    IN job INT,
    IN type INT,
    IN level INT,
    IN skillPoint INT,
    IN skillArray JSON
)
BEGIN
	DECLARE i INT DEFAULT 0;
    DECLARE size INT;
    DECLARE skillJson JSON;
    
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
		SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'A custom error occurred during the transaction.';
    END;
    
    START TRANSACTION;
    
    INSERT INTO characters_job
		(`cid`, `id`, `type`, `level`, `skill_point`)
    VALUES 
		(cid, job, type, level, skillPoint);
    
	SET size = JSON_LENGTH(skillArray);
	
	WHILE i < size DO 
		SET skillJson = JSON_EXTRACT(skillArray, CONCAT('$[', i, ']'));
        
        INSERT INTO characters_skill
			( `cid`, `id`, `job`, `level` )
		VALUES (
			JSON_EXTRACT(skillJson, '$.cid'),
			JSON_EXTRACT(skillJson, '$.id'),
			JSON_EXTRACT(skillJson, '$.job'),
			JSON_EXTRACT(skillJson, '$.level')
        );
    
		SET i = i + 1;
    
    END WHILE;
    
    COMMIT;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_job_exp_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_job_exp_set`(
	IN cid BIGINT,
    IN job INT,
    IN exp INT
)
BEGIN
	UPDATE characters_job SET exp = exp WHERE characters_job.cid = cid and characters_job.id = job;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_job_level_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_job_level_set`(
	IN cid BIGINT,
    IN job INT,
    IN level INT,
    IN skillPoint INT,
    IN skillArray JSON
)
BEGIN
	DECLARE i INT DEFAULT 0;
    DECLARE size INT;
    DECLARE skillJson JSON;
    
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
		SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'A custom error occurred during the transaction.';
    END;
    
    START TRANSACTION;
    
    UPDATE characters_job
    SET level = level, exp = 0, skill_point = skillPoint
    WHERE characters_job.cid = cid and characters_job.id = job;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000';
	END IF;
    
	SET size = JSON_LENGTH(skillArray);
	
	WHILE i < size DO 
		SET skillJson = JSON_EXTRACT(skillArray, CONCAT('$[', i, ']'));
        
        INSERT INTO characters_skill
			( `cid`, `id`, `job`, `level` )
		VALUES (
			JSON_EXTRACT(skillJson, '$.cid'),
			JSON_EXTRACT(skillJson, '$.id'),
			JSON_EXTRACT(skillJson, '$.job'),
			JSON_EXTRACT(skillJson, '$.level')
        );
    
		SET i = i + 1;
    
    END WHILE;
    
    COMMIT;
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_level_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_level_set`(
	IN cid BIGINT,
    IN level INT,
    IN statPoint INT
)
BEGIN
	UPDATE characters_stat 
    SET chr_lv = level, chr_exp = 0, stat_point = statPoint
    WHERE characters_stat.cid = cid;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_mix_skill_exp_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_mix_skill_exp_set`(
	IN cid BIGINT,
    IN skillId INT,
    IN skillLevel INT,
    IN exp INT
)
BEGIN
	UPDATE characters_skill
    SET level = skillLevel, exp = exp
    WHERE characters_skill.cid = cid AND characters_skill.id = skillId;

	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_profile_introduction_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_profile_introduction_get`(
	IN cid BIGINT
)
BEGIN

	SELECT age, sex, mail, message
    FROM characters_profile_introduction as cpi	
    WHERE cpi.cid = cid;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_profile_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_profile_set`(
	IN cid BIGINT,
    IN refusePartyInvite TINYINT,
    IN refuseChannelInvite TINYINT,
    IN refuseGuildInvite TINYINT,
    IN privateProfile TINYINT,
    IN age VARCHAR(256),
    IN sex VARCHAR(256),
    IN mail VARCHAR(256),
    IN message VARCHAR(2048)
)
BEGIN

	INSERT INTO characters_profile_setting
		(`cid`, `refuse_party_invite`, `refuse_channel_invite`, `refuse_guild_invite`, `private`)
	VALUES
		(cid, refusePartyInvite, refuseChannelInvite, refuseGuildInvite, privateProfile)
	ON DUPLICATE KEY UPDATE
		refuse_party_invite = VALUES(refuse_party_invite),
		refuse_channel_invite = VALUES(refuse_channel_invite),
		refuse_guild_invite = VALUES(refuse_guild_invite),
		private = VALUES(private);

	INSERT INTO characters_profile_introduction
		(`cid`, `age`, `sex`, `mail`, `message`)
	VALUES 
		(cid, age, sex, mail, message)
	ON DUPLICATE KEY UPDATE
		age = VALUES(age),
		sex = VALUES(sex),
		mail = VALUES(mail),
		message = VALUES(message);

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_quest_add` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_quest_add`(
	IN cid BIGINT,
    IN id INT,
    IN state INT,
    IN flags VARCHAR(200),
    IN data VARCHAR(200)
)
BEGIN
	INSERT INTO characters_quest
		(`cid`, `id`, `state`, `flags`, `data`)
	VALUES
		(cid, id, state, flags, data);

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_quest_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_quest_set`(
	IN cid BIGINT,
    IN id INT,
    IN state INT,
    IN flags VARCHAR(200),
    IN data VARCHAR(200)
)
BEGIN
	UPDATE characters_quest
		SET state = state, flags = flags, data = data
	WHERE characters_quest.cid = cid and characters_quest.id = id;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_skill_add` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_skill_add`(
	IN cid BIGINT,
    IN job INT,
    IN skill INT,
    IN level INT
)
BEGIN
	INSERT INTO characters_skill
		(`cid`, `job`, `id`, `level`)
	VALUES
		(cid, job, skill, level);

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_skill_level_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_skill_level_set`(
	IN cid BIGINT,
    IN job INT,
    IN skillPoint INT,
    IN skillId INT,
    IN skillLevel INT
)
BEGIN
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
		SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'A custom error occurred.';
    END;
    
    START TRANSACTION;
	
	UPDATE characters_job
    SET skill_point = skillPoint
    WHERE characters_job.cid = cid and characters_job.id = job;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000';
	END IF;
    
    UPDATE characters_skill
    SET level = skillLevel
    WHERE characters_skill.cid = cid and characters_skill.id = skillId;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000';
	END IF;
    
    COMMIT;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_skill_position_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_skill_position_set`(
	IN _cid BIGINT,
    IN _id INT,
    IN _page TINYINT,
    IN _x TINYINT,
    IN _y TINYINT
)
BEGIN
	UPDATE characters_skill
    SET page = _page, x = _x, y = _y
    WHERE characters_skill.cid = _cid AND characters_skill.id = _id;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_state_save` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_state_save`(
	IN cid BIGINT,
    IN zone INT,
    IN stage INT,
    IN x FLOAT, 
    IN y FLOAT,
    IN yaw FLOAT,
    IN arms TINYINT,
    IN running TINYINT,
    IN dead TINYINT,
    IN hp INT,
    IN sp INT
)
BEGIN
	UPDATE characters
    SET zone = zone, stage = stage, x = x, y = y, yaw = yaw, arms = arms, running = running, dead = dead
    WHERE characters.id = cid;
    
    UPDATE characters_stat
    SET hp = hp, sp = sp
    WHERE characters_stat.cid = cid;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `character_stat_set` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `character_stat_set`(
	IN cid BIGINT,
    IN statPoint INT,
    IN str INT,
    IN dex INT,
    IN accr INT,
    IN health INT,
    IN intell INT,
    IN wis INT,
    IN will INT
)
BEGIN
	UPDATE characters_stat
    SET stat_point = statPoint, str = str, dex = dex, accr = accr, health = health, intell = intell, wis = wis, will = will
    WHERE characters_stat.cid = cid;
    
	IF ROW_COUNT() != 1 THEN
		SIGNAL SQLSTATE '45000' 
		SET MESSAGE_TEXT = 'update count error';
	END IF;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `item_transaction_execute` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `item_transaction_execute`(
	IN jsonArray JSON
)
BEGIN
	DECLARE i INT DEFAULT 0;
    DECLARE size INT;
    DECLARE type INT;
    DECLARE element JSON;
    
    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
		SIGNAL SQLSTATE '45000' 
        SET MESSAGE_TEXT = 'A custom error occurred during the item transaction.';
    END;
    
    SET size = JSON_LENGTH(jsonArray);
    
 	WHILE i < size DO 
		SET element = JSON_EXTRACT(jsonArray, CONCAT('$[', i, ']'));
		SET type = JSON_EXTRACT(element, '$.type');

        CASE type
        
			WHEN 1 THEN 
				UPDATE
					characters_item
                SET 
					pos_type = JSON_UNQUOTE(JSON_EXTRACT(element, '$.posType')),
                    page = JSON_EXTRACT(element, '$.page'),
                    x = JSON_EXTRACT(element, '$.x'),
                    y = JSON_EXTRACT(element, '$.y')
				WHERE
					id = JSON_EXTRACT(element, '$.id');
    
                IF ROW_COUNT() != 1 THEN
					SIGNAL SQLSTATE '45000';
                END IF;
                
			WHEN 2 THEN 
				UPDATE
					characters_item
                SET 
                    quantity = JSON_EXTRACT(element, '$.quantity')
				WHERE
					id = JSON_EXTRACT(element, '$.id');
				
                IF ROW_COUNT() = 0 THEN
					SIGNAL SQLSTATE '45000';
                END IF;
                
			WHEN 3 THEN 

				INSERT INTO characters_item
					(id, cid, item_id, quantity, pos_type, page, x, y)
				VALUES (
					JSON_EXTRACT(element, '$.id'),
					JSON_EXTRACT(element, '$.cid'),
					JSON_EXTRACT(element, '$.itemId'),
					JSON_EXTRACT(element, '$.quantity'),
					JSON_UNQUOTE(JSON_EXTRACT(element, '$.posType')),
					JSON_EXTRACT(element, '$.page'),
					JSON_EXTRACT(element, '$.x'),
					JSON_EXTRACT(element, '$.y')
				);
        
			WHEN 4 THEN 
				DELETE FROM characters_item
                WHERE id = JSON_EXTRACT(element, '$.id');
                
			WHEN 5 THEN 
				UPDATE characters
                SET gold = JSON_EXTRACT(element, '$.gold')
                WHERE id = JSON_EXTRACT(element, '$.cid');
                
			WHEN 6 THEN
				INSERT INTO accounts_storage_item
					(id, aid, item_id, quantity, page, x, y)
				VALUES (
					JSON_EXTRACT(element, '$.id'),
					JSON_EXTRACT(element, '$.aid'),
					JSON_EXTRACT(element, '$.itemId'),
					JSON_EXTRACT(element, '$.quantity'),
					JSON_EXTRACT(element, '$.page'),
					JSON_EXTRACT(element, '$.x'),
					JSON_EXTRACT(element, '$.y')
				);
                
			WHEN 7 THEN
				DELETE FROM accounts_storage_item
                WHERE id = JSON_EXTRACT(element, '$.id');

        END CASE;

        SET i = i + 1;

    END WHILE;

    COMMIT;   
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `lobby_characters_get` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_0900_ai_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`localhost` PROCEDURE `lobby_characters_get`(
	IN accountId BIGINT,
    IN _sid TINYINT
)
BEGIN
	CREATE TEMPORARY TABLE IF NOT EXISTS temp (
		id BIGINT
    );
    
    TRUNCATE temp;
    
    INSERT INTO temp
		( id )
	SELECT c.id FROM characters AS c WHERE c.aid = accountId AND c.deleted = 0 AND c.sid = _sid;

	SELECT 
		c.id,
        c.slot,
        c.name,
        cs.chr_lv,
        cs.gender,
        c.hair,
        c.hair_color,
        c.face,
        c.skin_color,
        c.zone,
        c.arms
    FROM
		characters AS c
	LEFT JOIN 
		characters_stat AS cs
	ON
		cs.cid = c.id
	WHERE
		c.id IN (SELECT id FROM temp);

    SELECT
		ci.cid,
        ci.item_id,
        ci.page
	FROM
		characters_item AS ci
	WHERE 
		ci.cid IN (SELECT id FROM temp) AND
        ci.pos_type = 'equipment' AND
        ci.page > 0 AND ci.page <= 5 # 5 -> Shoes 
        ;
        
    SELECT
		cj.cid,
        cj.id,
        cj.type
	FROM
		characters_job AS cj
	WHERE 
		cj.cid IN (SELECT id FROM temp) AND
        (
			cj.type = 0 or cj.type = 1 
        );
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2024-12-20  6:53:52
