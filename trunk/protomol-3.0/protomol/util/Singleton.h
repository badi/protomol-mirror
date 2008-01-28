#ifndef SINGLETON_H
#define SINGLETON_H

namespace ProtoMol {
  template <class T>
  class Singleton {
  protected:
    static T *singleton;

    Singleton() {}
    ~Singleton() {}

  public:
    static T *instance() {
      if (!singleton) singleton = new T;
      return singleton;
    }
  };

  template <class T>
  T *Singleton<T>::singleton = 0;
};

#endif // SINGLETON_H
