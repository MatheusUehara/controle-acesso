-- MySQL Workbench Forward Engineering

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL,ALLOW_INVALID_DATES';

-- -----------------------------------------------------
-- Schema ac
-- -----------------------------------------------------
DROP SCHEMA IF EXISTS `ac` ;

-- -----------------------------------------------------
-- Schema ac
-- -----------------------------------------------------
CREATE SCHEMA IF NOT EXISTS `ac` DEFAULT CHARACTER SET utf8 COLLATE utf8_general_ci ;
USE `ac` ;

-- -----------------------------------------------------
-- Table `ac`.`User`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ac`.`User` ;

CREATE TABLE IF NOT EXISTS `ac`.`User` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `rfid` VARCHAR(45) NOT NULL COMMENT '',
  `name` VARCHAR(45) NOT NULL COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '',
  UNIQUE INDEX `rfid_UNIQUE` (`rfid` ASC)  COMMENT '')
ENGINE = InnoDB;


-- -----------------------------------------------------
-- Table `ac`.`History`
-- -----------------------------------------------------
DROP TABLE IF EXISTS `ac`.`History` ;

CREATE TABLE IF NOT EXISTS `ac`.`History` (
  `id` INT NOT NULL AUTO_INCREMENT COMMENT '',
  `idUser` INT NOT NULL COMMENT '',
  `entry` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '',
  `saida` VARCHAR(45) NULL DEFAULT NULL COMMENT '',
  `status` CHAR NOT NULL DEFAULT 0 COMMENT '',
  PRIMARY KEY (`id`)  COMMENT '',
  INDEX `fk_History_User_idx` (`idUser` ASC)  COMMENT '',
  CONSTRAINT `fk_History_User`
    FOREIGN KEY (`idUser`)
    REFERENCES `ac`.`User` (`id`)
    ON DELETE NO ACTION
    ON UPDATE NO ACTION)
ENGINE = InnoDB;


SET SQL_MODE=@OLD_SQL_MODE;
SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS;
SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS;

-- -----------------------------------------------------
-- Data for table `ac`.`User`
-- -----------------------------------------------------
START TRANSACTION;
USE `ac`;
INSERT INTO `ac`.`User` (`id`, `rfid`, `name`) VALUES (DEFAULT, 'ae43d3b5', 'Guilherme Araujo');
INSERT INTO `ac`.`User` (`id`, `rfid`, `name`) VALUES (DEFAULT, 'd798b4f5', 'Matheus Uehara');
INSERT INTO `ac`.`User` (`id`, `rfid`, `name`) VALUES (DEFAULT, 'b3f7729a', 'Francois Michell');

COMMIT;