/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            Время работы после старта или рестарта
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
String upTime(uint32_t lasttime) {
  
  lasttime /= 1000;
  String lastStartTime;

  if (lasttime > 86400) {
    uint8_t lasthour = lasttime / 86400;
    lastStartTime.concat(lasthour);
    lastStartTime.concat(F("d "));
    lasttime = (lasttime - (86400 * lasthour));
  }
  if (lasttime > 3600) {
 /*   if (lasttime / 3600 < 10) {
      lastStartTime.concat(F("0"));
    }*/
    lastStartTime.concat(lasttime / 3600);
    lastStartTime.concat(F("ч "));
  }
  if (lasttime / 60 % 60 < 10) {
    lastStartTime.concat(F("0"));
  }
  lastStartTime.concat((lasttime / 60) % 60);
  lastStartTime.concat(F(":"));
  if (lasttime % 60 < 10) {
    lastStartTime.concat(F("0"));
  }
  lastStartTime.concat(lasttime % 60);
  //lastStartTime.concat(F("s"));

  return lastStartTime;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
