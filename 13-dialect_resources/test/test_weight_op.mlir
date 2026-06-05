// Test: a siren.weight op referencing a resource blob parses and roundtrips cleanly.
// Run: install/bin/ch-13-opt %s | install/bin/ch-13-opt

{-#
  dialect_resources: {
    siren: {
      w0: "0x04000000DEADBEEF"
    }
  }
#-}

func.func @load_weight() -> i8 {
  %0 = siren.weight <w0> : i8
  func.return %0 : i8
}
