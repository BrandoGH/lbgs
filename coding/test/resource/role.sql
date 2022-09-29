/*
 Navicat Premium Data Transfer

 Source Server         : local_debian_postgres
 Source Server Type    : PostgreSQL
 Source Server Version : 140005
 Source Host           : 192.168.43.122:5432
 Source Catalog        : postgres
 Source Schema         : public

 Target Server Type    : PostgreSQL
 Target Server Version : 140005
 File Encoding         : 65001

 Date: 29/09/2022 15:25:14
*/


-- ----------------------------
-- Table structure for role
-- ----------------------------
DROP TABLE IF EXISTS "public"."role";
CREATE TABLE "public"."role" (
  "name" varchar(255) COLLATE "pg_catalog"."default",
  "age" int4
)
;

-- ----------------------------
-- Records of role
-- ----------------------------
INSERT INTO "public"."role" VALUES ('liubin', 12);
INSERT INTO "public"."role" VALUES ('dengtong', 10);
INSERT INTO "public"."role" VALUES ('jacek', 30);
INSERT INTO "public"."role" VALUES ('steady', 700);
