package main

import (
	"fmt"
	"os"
)

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("Usage: %v <plaintext>\n", os.Args[0])
		os.Exit(1)
	}

	runAES(os.Args[1])
}
