// Test: a {-# dialect_resources #-} section for 'siren' parses and roundtrips cleanly.
// Run: install/bin/ch-13-opt %s | install/bin/ch-13-opt

{-#
  dialect_resources: {
    siren: {
      my_weight: "0x08000000010000000000000002000000"
    }
  }
#-}

func.func @noop() {
  func.return
}
