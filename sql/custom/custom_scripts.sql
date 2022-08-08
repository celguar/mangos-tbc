-- ----------------------------
-- Table structure for custom_scripts
-- ----------------------------
DROP TABLE IF EXISTS `custom_scripts`;
CREATE TABLE `custom_scripts`  (
  `id` int UNSIGNED NOT NULL AUTO_INCREMENT,
  `ScriptName` varchar(128) CHARACTER SET utf8mb3 COLLATE utf8mb3_general_ci NOT NULL DEFAULT '',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = MyISAM AUTO_INCREMENT = 2 CHARACTER SET = utf8mb3 COLLATE = utf8mb3_general_ci ROW_FORMAT = Dynamic;
