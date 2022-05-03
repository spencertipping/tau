package tau::map;

use Encode       qw/decode/;
use List::Util   qw/max/;
use Scalar::Util qw/openhandle/;

use constant news_vectors => {n => [ 0, -1],
                              e => [ 1,  0],
                              w => [-1,  0],
                              s => [ 0,  1]};


sub news
{
  my ($s)     = @_;
  my ($x, $y) = (0, 0);
  for (split //, $s)
  {
    my ($dx, $dy) = news_vectors->{$_};
    $x += $dx;
    $y += $dy;
  }
  ($x, $y);
}


sub new
{
  my ($class, $f) = @_;
  my %m;
  my $y  = 0;
  my $mx = 0;

  open my $fh, "<", $f or die "open < $f: $!";

  while (defined(my $l = <$fh>))
  {
    chomp $l;
    $l = decode 'UTF-8', $l;
    $m{pack LL => $_, $y} = ord substr $l, $_, 1 for 0..length($l)-1;
    $mx = max $mx, length($l) - 1;
    ++$y;
  }

  $m{mx} = $mx;
  $m{my} = $y - 1;

  # Fill voids with spaces so strings will be populated regardless of
  # whitespace formatting
  for my $x (0..$m{mx}) { $m{pack LL => $x, $_} //= 32 for 0..$m{my} }

  bless \%m, $class;
}


sub n      { scalar(keys %{+shift}) - 2 }
sub bounds { @{+shift}{qw/mx my/} }
sub at     { ${+shift}{pack LL => @_} }


sub has
{
  my ($self, $x, $y) = @_;
  my $n = $self->at($x, $y);
  defined $n && $n != 32;
}


sub str
{
  my $self = shift;
  my ($mx, $my) = $self->bounds;
  my @ls;
  for my $y (0..$my)
  {
    push @ls, join"", map chr, grep defined, map $self->at($_, $y), 0..$mx;
  }
  join"\n", @ls;
}


sub next
{
  my ($self, $x, $y, $news) = @_;
  my ($dx, $dy) = news $news;
  my ($mx, $my) = $self->bounds;

  $x += $dx;
  $y += $dy;

  while ($x >= 0 && $x <= $mx && $y >= 0 && $y <= $my)
  {
    my $c = $self->at($x, $y);
    return ($x, $y, chr $c) if defined $c && $c != 32;
    $x += $dx;
    $y += $dy;
  }
}


sub sub
{
  my ($self, $x, $y, $news, $n) = @_;
  my ($dx, $dy) = news $news;
  my $s = '';
  for (1..$n)
  {
    $s .= chr $self->at($x, $y);
    $x += $dx;
    $y += $dy;
  }
  $s;
}
