#ifndef _HTML_CORE_INITIALIZER_H_
#define _HTML_CORE_INITIALIZER_H_


namespace html {

class CoreInitializer {
public:
  static CoreInitializer& GetInstance();

  void Initialize();

};

}


#endif