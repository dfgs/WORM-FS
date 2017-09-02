
CREATE DATABASE  IF NOT EXISTS `WORM` /*!40100 DEFAULT CHARACTER SET utf8 */;

grant  EXECUTE ON PROCEDURE `WORM`.WriteAudit to 'audit'@'%' identified by 'audit1708$';
grant ALL on WORM.* to 'administrator'@'%' identified by 'worm1708$';

USE `WORM`;
-- MySQL dump 10.13  Distrib 5.6.17, for Win64 (x86_64)
--
-- Host: 10.0.0.3    Database: WORM
-- ------------------------------------------------------
-- Server version	5.6.22

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Audits`
--

DROP TABLE IF EXISTS `Audits`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Audits` (
  `AuditID` int(10) NOT NULL AUTO_INCREMENT,
  `RepositoryID` smallint(5) NOT NULL,
  `DateTime` datetime NOT NULL,
  `Operation` enum('CREATE','DELETE','UPDATE') NOT NULL,
  `Entity` enum('FILE','DIRECTORY','EXPIRATION','RETENTION','OWNER','MODE','TIME','LOCATION') NOT NULL,
  `Result` enum('SUCCESS','FAILURE') NOT NULL,
  `FileName` longtext NOT NULL,
  `Value` longtext NOT NULL,
  `UID` smallint(5) unsigned NOT NULL DEFAULT '0',
  `GID` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`AuditID`)
) ENGINE=InnoDB AUTO_INCREMENT=3249 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `Entries`
--

DROP TABLE IF EXISTS `Entries`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Entries` (
  `EntryID` int(11) NOT NULL AUTO_INCREMENT,
  `RepositoryID` smallint(5) NOT NULL,
  `Path` longtext NOT NULL,
  `Retention` smallint(6) NOT NULL DEFAULT '0',
  `ExpirationDate` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `IsDeleted` tinyint(1) NOT NULL DEFAULT '0',
  PRIMARY KEY (`EntryID`)
) ENGINE=InnoDB AUTO_INCREMENT=107 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `EntriesView`
--

DROP TABLE IF EXISTS `EntriesView`;
/*!50001 DROP VIEW IF EXISTS `EntriesView`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
/*!50001 CREATE TABLE `EntriesView` (
  `EntryID` tinyint NOT NULL,
  `RepositoryID` tinyint NOT NULL,
  `Path` tinyint NOT NULL,
  `Retention` tinyint NOT NULL,
  `ExpirationDate` tinyint NOT NULL,
  `IsDeleted` tinyint NOT NULL,
  `IsExpired` tinyint NOT NULL
) ENGINE=MyISAM */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `Stats`
--

DROP TABLE IF EXISTS `Stats`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Stats` (
  `StatID` int(11) NOT NULL AUTO_INCREMENT,
  `RepositoryID` smallint(5) NOT NULL,
  `DateTime` datetime NOT NULL,
  `NumberOfExpiredEntries` int(11) NOT NULL,
  `NumberOfNonExpiredEntries` int(11) NOT NULL,
  `NumberOfEntries` int(11) NOT NULL,
  `NumberOfDeletedEntries` int(11) NOT NULL,
  PRIMARY KEY (`StatID`)
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping events for database 'WORM'
--
/*!50106 SET @save_time_zone= @@TIME_ZONE */ ;
/*!50106 DROP EVENT IF EXISTS `UpdateStats` */;
DELIMITER ;;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;;
/*!50003 SET character_set_client  = utf8 */ ;;
/*!50003 SET character_set_results = utf8 */ ;;
/*!50003 SET collation_connection  = utf8_general_ci */ ;;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;;
/*!50003 SET @saved_time_zone      = @@time_zone */ ;;
/*!50003 SET time_zone             = 'SYSTEM' */ ;;
/*!50106 CREATE*/ /*!50117 DEFINER=`administrator`@`%`*/ /*!50106 EVENT `UpdateStats` ON SCHEDULE EVERY 1 DAY STARTS '2010-06-20 00:00:00' ON COMPLETION NOT PRESERVE ENABLE DO call UpdateStats(1) */ ;;
/*!50003 SET time_zone             = @saved_time_zone */ ;;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;;
/*!50003 SET character_set_client  = @saved_cs_client */ ;;
/*!50003 SET character_set_results = @saved_cs_results */ ;;
/*!50003 SET collation_connection  = @saved_col_connection */ ;;
DELIMITER ;
/*!50106 SET TIME_ZONE= @save_time_zone */ ;

--
-- Dumping routines for database 'WORM'
--
/*!50003 DROP FUNCTION IF EXISTS `CreateEntryFromPath` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` FUNCTION `CreateEntryFromPath`(
	RepositoryID smallint,
    Entity ENUM('FILE','DIRECTORY','EXPIRATION','RETENTION','OWNER','MODE','TIME','LOCATION'), 
	Path longtext 
) RETURNS int(11)
BEGIN
	declare id integer;
    
    set id=null;
	if (Entity='FILE' or Entity='DIRECTORY') THEN 
		set id=GetEntryIDFromPath(RepositoryID,Path,0);
        if isnull(id) THEN
			insert into Entries(RepositoryID,Path) values (RepositoryID,Path);
            select LAST_INSERT_ID() into id;
		END IF;
    END IF;


RETURN id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP FUNCTION IF EXISTS `DeleteEntryFromPath` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` FUNCTION `DeleteEntryFromPath`(
	RepositoryID smallint,
    Entity ENUM('FILE','DIRECTORY','EXPIRATION','RETENTION','OWNER','MODE','TIME','LOCATION'), 
	Path longtext 

) RETURNS int(11)
BEGIN
	declare id int;
	
    set id=null;
	if (Entity='FILE' or Entity='DIRECTORY') THEN 
		set id=CreateEntryFromPath(RepositoryID,Entity,Path);
		CALL DeleteEntryFromID(id);
	END IF;
    
    RETURN id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP FUNCTION IF EXISTS `GetEntryIDFromPath` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` FUNCTION `GetEntryIDFromPath`(RepositoryID smallint,Path longtext,ReturnDeleted tinyint(1)) RETURNS int(11)
BEGIN
	declare id integer;
    
    set id=null;
    if (ReturnDeleted=1) then
        select EntryID into id from Entries where Entries.Path=Path and Entries.RepositoryID=RepositoryID LIMIT 1;
    else
        select EntryID into id from Entries where Entries.Path=Path and Entries.RepositoryID=RepositoryID and IsDeleted=0 LIMIT 1;
    end if;
    
	RETURN id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP FUNCTION IF EXISTS `UpdateEntryFromPath` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` FUNCTION `UpdateEntryFromPath`(
	RepositoryID smallint,
    Entity ENUM('FILE','DIRECTORY','EXPIRATION','RETENTION','OWNER','MODE','TIME','LOCATION'), 
    Path longtext ,
    Value longtext
) RETURNS int(11)
BEGIN

	declare id int;
    
    set id=GetEntryIDFromPath(RepositoryID,Path,0);

	case Entity
		WHEN 'EXPIRATION' then CALL UpdateExpirationFromID(id,Value);
		WHEN 'RETENTION' then CALL UpdateRetentionFromID(id,Value);
		WHEN 'LOCATION' then CALL UpdateLocationFromID(id,Value);
        ELSE BEGIN END;
	END CASE;
    
	RETURN id;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `DeleteEntryFromID` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `DeleteEntryFromID`(in EntryID int)
BEGIN
	Update Entries set IsDeleted=1 where Entries.EntryID=EntryID;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `UpdateExpirationFromID` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `UpdateExpirationFromID`(in EntryID int,in Value longtext)
BEGIN
	declare seconds int;
    declare expiration datetime;
    
    set seconds=CONVERT(Value,unsigned);
    set expiration=FROM_UNIXTIME(seconds);
    
	update Entries set ExpirationDate=expiration where Entries.EntryID=EntryID;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `UpdateLocationFromID` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `UpdateLocationFromID`(in EntryID int,in NewPath longtext)
BEGIN
    
   	Update Entries set Path=NewPath where Entries.EntryID=EntryID;
    

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `UpdateRetentionFromID` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `UpdateRetentionFromID`(in EntryID int,in Value longtext)
BEGIN

	update Entries set Retention=Value where Entries.EntryID=EntryID;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `UpdateStats` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `UpdateStats`(RepositoryID smallint)
BEGIN
	declare numberOfEntries int;
	declare numberOfExpiredEntries int;
	declare numberOfNonExpiredEntries int;
	declare numberOfDeletedEntries int;
    
    SELECT count(EntryID) into numberOfNonExpiredEntries FROM EntriesView where IsExpired=0 and IsDeleted=0 and EntriesView.RepositoryID=RepositoryID;
    SELECT count(EntryID) into numberOfExpiredEntries FROM EntriesView where IsExpired=1 and IsDeleted=0 and EntriesView.RepositoryID=RepositoryID;
    set numberOfEntries=numberOfExpiredEntries+numberOfNonExpiredEntries;
    SELECT count(EntryID) into numberOfDeletedEntries FROM EntriesView where IsDeleted=1 and EntriesView.RepositoryID=RepositoryID;

	insert into Stats (RepositoryID,DateTime,NumberOfExpiredEntries,NumberOfNonExpiredEntries,NumberOfEntries,NumberOfDeletedEntries) values (RepositoryID,NOW(),numberOfExpiredEntries,numberOfNonExpiredEntries,numberOfEntries,numberOfDeletedEntries);

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `WriteAudit` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8 */ ;
/*!50003 SET character_set_results = utf8 */ ;
/*!50003 SET collation_connection  = utf8_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'STRICT_TRANS_TABLES,NO_ENGINE_SUBSTITUTION' */ ;
DELIMITER ;;
CREATE DEFINER=`administrator`@`%` PROCEDURE `WriteAudit`(
	IN DateTime datetime ,
    IN RepositoryID smallint,
    IN Operation ENUM('CREATE','DELETE','UPDATE'),
    in Entity ENUM('FILE','DIRECTORY','EXPIRATION','RETENTION','OWNER','MODE','TIME','LOCATION'), 
    in Result ENUM('SUCCESS','FAILURE') ,
    in FileName longtext ,
    in Value longtext ,
	in UID smallint ,
    in GID  smallint)
BEGIN
	insert into Audits (Datetime,RepositoryID,Operation,Entity,Result,FileName,Value,UID,GID) 
    values (DateTime,RepositoryID,Operation,Entity,Result,FileName,Value,UID,GID);

    if (Result!='FAILURE') then
		case Operation
			WHEN 'CREATE' then select CreateEntryFromPath(RepositoryID,Entity,FileName);
			WHEN 'DELETE' then select DeleteEntryFromPath(RepositoryID,Entity,FileName);
			WHEN 'UPDATE' then select UpdateEntryFromPath(RepositoryID,Entity,FileName,Value);
            ELSE BEGIN END;
		END CASE;
    END IF;
    
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Final view structure for view `EntriesView`
--

/*!50001 DROP TABLE IF EXISTS `EntriesView`*/;
/*!50001 DROP VIEW IF EXISTS `EntriesView`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8 */;
/*!50001 SET character_set_results     = utf8 */;
/*!50001 SET collation_connection      = utf8_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`administrator`@`%` SQL SECURITY DEFINER */
/*!50001 VIEW `EntriesView` AS select `Entries`.`EntryID` AS `EntryID`,`Entries`.`RepositoryID` AS `RepositoryID`,`Entries`.`Path` AS `Path`,`Entries`.`Retention` AS `Retention`,`Entries`.`ExpirationDate` AS `ExpirationDate`,`Entries`.`IsDeleted` AS `IsDeleted`,if(((to_days(now()) - to_days(`Entries`.`ExpirationDate`)) < 0),0,1) AS `IsExpired` from `Entries` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-04-18 11:08:05



