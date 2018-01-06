# Multithread downloader

A multi-threaded HTTP downloader program.

### About

The program consists of three parts:

**http.c** - performs an HTTP 1.0 query to a website and returns the response string (including header and page content).

**queue.c** - A concurrent FIFO queue which allows multiple producers and consumers to communicate in a thread-safe way. 

**downloader.c** - The main component of the program. Spawns worker threads that consume tasks from the queue and perform queries.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

What things you need to install the software and how to install them

```
sudo apt-get update
sudo apt-get install build-essential
git clone https://github.com/JordanCleeve/multithread-downloader.git
cd multithread-downloader
```

### Compiling

Run the command from the project's root directory. The complied files will be placed in the root directory.

```
make
```

## Usage

To use the project create a .txt file with a url per line and run the line of code

```
./downloader 'url file name' 'number of threads' 'directory'
```


## Running the tests

There are three tests provided

### http_test

This test is for the http.c file and will output the header and content from host/page

```
./http_test 'host' 'page'
```

### http_download

http_download is similar to http_test but instead writes the downloaded file to disk using a filename you give it. 

```
./test_download 'url' 'filename'
```

### test_download

A shell script to compare the output of the http_download against wget

```
./test_download url
```

## Contributing

When contributing to this repository, please first discuss the change you wish to make via issue, email, or any other method with the owners of this repository before making a change.

## Authors

* **Jordan Cleeve** - *Initial work* - [JordanCleeve](https://github.com/JordanCleeve)
* **University of Canterbury Department of Computer Science and Software Engineering** - *downloader.c, tests*

## License

This project is licensed under the Attribution-NonCommercial 3.0 New Zealand (CC BY-NC 3.0 NZ) License - see the [LICENSE.md](LICENSE.md) file for details

