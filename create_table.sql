-- phpMyAdmin SQL Dump
-- version 5.2.1
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Sep 09, 2025 at 02:34 PM
-- Server version: 5.7.23-23
-- PHP Version: 8.1.32

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";


--
-- Table structure for table `sensor_data`
--

CREATE TABLE `sensor_data` (
                               `id` int(11) NOT NULL,
                               `sensor_type` varchar(32) COLLATE utf8_unicode_ci NOT NULL,
                               `sensor_name` varchar(16) COLLATE utf8_unicode_ci NOT NULL,
                               `value` float NOT NULL,
                               `unit` varchar(16) COLLATE utf8_unicode_ci NOT NULL,
                               `device` varchar(16) COLLATE utf8_unicode_ci NOT NULL,
                               `timestamp` datetime DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Indexes for table `sensor_data`
--
ALTER TABLE `sensor_data`
    ADD PRIMARY KEY (`id`);

--
-- AUTO_INCREMENT for table `sensor_data`
--
ALTER TABLE `sensor_data`
    MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1;
COMMIT;
