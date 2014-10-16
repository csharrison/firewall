import java.util.zip.CRC32;
import java.util.zip.Checksum;
import java.lang.Object;
import java.lang.Math;

// test app to demonstrate how work is distributed between uniform and 
// exponential packets

class UniformGenerator {
  RandomGenerator randGen;
  public UniformGenerator(int seed) {
    randGen = new RandomGenerator(seed);
  }
  public UniformGenerator() {
    randGen = new RandomGenerator();
  }
  int getRand(int minValue, int maxValue) { // [minValue,maxValue) like indexing
    return ( randGen.getRand()  % (maxValue-minValue) ) + minValue;
  }
  int getRand(int maxValue) { // [0,maxValue)
    return randGen.getRand() % maxValue;
  }
  int getRand() {
    return randGen.getRand();
  }
  double getUnitRand() {
    final double base_d = 1073741824.0d; // 2^30
    final int base_i = 1073741824; // 2^30
    return ((double) getRand(base_i))/base_d;
  }
  void setSeed(int startSeed) {
    randGen.setSeed(startSeed);
  }
  public int mangle(int seed) { return randGen.mangle(seed); }
}

class ExponentialGenerator {
  final double mean;
  final RandomGenerator randGen;
  final double logBase = 20.795; // ln(2^30 - 1)
  final int base = 1073741824; // 2^30
  public ExponentialGenerator(double mean) {
    this.mean = mean;
    randGen = new RandomGenerator();
  }
  int getRand() {
    return (int) Math.ceil(mean*(logBase-Math.log(base-randGen.getRand())));
  }
  int getRand(double meanTmp) {
    return (int) Math.ceil(meanTmp*(logBase-Math.log(base-randGen.getRand())));
  }
  public int mangle(int seed) { return randGen.mangle(seed); }
}

class RandomGenerator {
  int seed;
  public RandomGenerator(int seed) {
    this.seed = seed;
  }
  public RandomGenerator() {
    this.seed = 59009;
  }
  int getRand() {
    seed = mangle(seed) + 1;
    return seed;
  }
  void setSeed(int seed) {
    this.seed = seed;
  }
  public int mangle(int seed) { 
    final int CRC_POLY = 954680065; // 0x38E74301 - standard CRC30 from CDMA
    final int iterations = 31;
    int crc = seed;
    for( int i = 0; i < iterations; i++ ) {
      if( ( crc & 1 ) > 0 )
        crc = (crc >> 1) ^ CRC_POLY;
      else
        crc = crc >> 1;
    }
    return crc;
  }
}

