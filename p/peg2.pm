package tau::peg2;


sub all
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = $_->($m, $x, $y)) || return undef for @ps;
    @r;
  };
}


sub any
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    defined(@r = $_->($m, $x, $y)) && return @r for @ps;
    undef;
  };
}


sub dspr(\%$)
{
  my ($ps, $news) = @_;
  my ($dx, $dy) = tau::map::news $news;
  sub
  {
    my ($m, $x, $y) = @_;
    for my $k (sort {length $b <=> length $a} keys %$ps)
    {
      # TODO
    }
  };
}


sub k
{
  my ($s, $news) = @_;
  my ($dx, $dy)  = tau::map::news $news;
  my $n          = length $s;
  sub
  {
    my ($m, $x, $y) = @_;
    $m->sub($x, $y, $n) eq $s
        ? ($x + $dx * $n, $y + $dy * $n, $s)
        : undef;
  };
}


sub seq
{
  my @ps = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    my @rvs;
    for (@ps)
    {
      return undef unless defined(@r = &$_($m, $x, $y));
      $x = shift @r;
      $y = shift @r;
      push @rvs, @r;
    }
    ($x, $y, @rvs);
  };
}


sub rep
{
  my ($p, $max) = (@_, -1 | 0);
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    my @rvs;
    while (defined(@r = &$p($m, $x, $y)))
    {
      $x = shift @r;
      $y = shift @r;
      push @rvs, @r;
    }
    ($x, $y, @rvs);
  };
}


sub bow    # beginning of word
{
  my ($news) = @_;
  my ($dx, $dy) = tau::map::news $news;
  sub
  {
    my ($m, $x, $y) = @_;
    $m->has($x - $dx, $y - $dy) ? undef : ($x, $y);
  };
}


sub pmap(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    ($x, $y, &$f(@r));
  };
}


sub pgrep(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    &$f(@r) ? ($x, $y, @r) : undef;
  };
}


sub pflatmap(&$)
{
  my ($f, $p) = @_;
  sub
  {
    my ($m, $x, $y) = @_;
    my @r;
    return undef unless defined(($x, $y, @r) = &$p($m, $x, $y));
    my $p2;
    return undef unless defined($p2 = &$f($m, $x, $y, @r));
    &$p2($m, $x, $y);
  };
}
