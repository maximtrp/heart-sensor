package main

import (
    "fmt"
    "io/ioutil"
    "math"
    "log"
    "strings"
    "time"
    "github.com/tarm/serial"
)

func main() {

    c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 115200, ReadTimeout: time.Millisecond}
    s, err := serial.OpenPort(c)
    if err != nil {
        log.Fatal(err)
    }

    fmt.Println("Port opened")

    n, err := s.Write([]byte("1"))
    if err != nil {
        log.Fatal(err)
    }

    result := make([]byte, int(math.Pow(2, 24)))
    buf := make([]byte, 16384)
    j := 0

    err = s.Flush()
    if err != nil {
        log.Fatal(err)
    }


    fmt.Println("Reading data")
    for i := 0; i < 150000; i++ {
        n, err = s.Read(buf)
        if err != nil {
            continue
        }
        for k := 0; k < n; k++ {
            result[j+k] = buf[k]
        }
        j += n
    }

    fmt.Println("Processing data")
    t := time.Now()
    fn := fmt.Sprintf("%04d-%02d-%02d %02d-%02d-%02d.csv", t.Year(), t.Month(), t.Day(), t.Hour(), t.Minute(), t.Second())

    result_str := string(result)
    x := strings.Index(result_str, "\r\n") + 2
    y := strings.LastIndex(result_str, "\r\n")
    result_clean := []byte(result_str[x:y])
    
    fmt.Println("Saving data")
    err = ioutil.WriteFile(fn, result_clean, 0644)
    if err != nil {
        log.Fatal(err)
    }
    
    defer s.Close()
}
