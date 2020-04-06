package main

import (
	"bytes"
	"compress/gzip"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"math"
	"strconv"
	"strings"
	"time"

	"github.com/tarm/serial"
)

func checkError(e error) {
	if e != nil {
		log.Fatal(e)
	}
}

func startSending(p *serial.Port) error {
	_, err := p.Write([]byte("1"))
	if err != nil {
		return err
	}
	return nil
}

func max(a, b int) int {
	if a >= b {
		return a
	}
	return b
}

func getTimeIndex(s *string, i, j int) int {
	t := strings.Split((*s)[i:j], " ")
	r, _ := strconv.Atoi(t[0])
	return r
}

func getProgress(data []byte, total int, lastByteIndex int) int {
	strBegin := string(data[:100])
	endIndex := max(0, lastByteIndex-100)
	strEnd := string(data[endIndex:lastByteIndex])

	i := strings.Index(strBegin, "\r\n") + 2
	j := strings.Index(strBegin[i:], "\r\n")
	t1 := getTimeIndex(&strBegin, i, i+j)

	j = strings.LastIndex(strEnd, "\r\n")
	i = strings.LastIndex(strEnd[:j], "\r\n") + 2
	t2 := getTimeIndex(&strEnd, i, j)
	return (t2 - t1) * 100 / total
}

func main() {
	total := flag.Int("t", 300000, "total record time in milliseconds")
	flag.Parse()

	result := make([]byte, int(math.Pow(2, 24)))
	buf := make([]byte, 16384)
	j := 0
	n := 0

	fmt.Println("[1] Port init")
	c := &serial.Config{Name: "/dev/ttyUSB0", Baud: 115200, ReadTimeout: time.Millisecond}
	s, err := serial.OpenPort(c)
	checkError(err)

	time.Sleep(2 * time.Second)

	// Sending bit to receive data
	fmt.Println("[2] Sending a start bit")
	err = startSending(s)
	checkError(err)

	// Reading data
	fmt.Println("[3] Getting data")
	for {
		n, err = s.Read(buf)
		if err != nil {
			continue
		}
		copy(result[j:j+n], buf[0:n])
		j += n
		progress := getProgress(result, *total, j)
		if progress >= 100 {
			fmt.Println("[4] Data loaded succesfully")
			break
		} else {
			fmt.Printf("[4] Loading: %d%%\r", progress)
		}
	}

	resultStr := string(result)
	x := strings.Index(resultStr, "\r\n") + 2
	y := strings.LastIndex(resultStr, "\r\n")
	resultClean := []byte(resultStr[x:y])

	t := time.Now()
	fn := t.Format("2006-01-02 15-04-05") + " ECG.csv.gz"

	var b bytes.Buffer
	w := gzip.NewWriter(&b)
	w.Write(resultClean)
	w.Close()

	err = ioutil.WriteFile(fn, b.Bytes(), 0644)
	checkError(err)

	defer s.Close()
}
